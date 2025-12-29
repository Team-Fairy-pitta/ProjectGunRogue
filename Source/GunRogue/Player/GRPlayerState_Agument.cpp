#include <rapidjson/document.h>

#include "Augment/GRAugmentDefinition.h"
#include "Player/GRPlayerState.h"
#include "Player/Battle/GRBattlePlayerController.h"
#include "Augment/GRAugmentStructs.h"
#include "Augment/GRAugmentSubsystem.h"
#include "MetaProgression/PerkInfoRow.h"


void AGRPlayerState::ServerRPC_OnAugmentSelected_Implementation(FName AugmentID)
{
	if (!HasAuthority())
	{
		return;
	}

	int32 FoundIndex = INDEX_NONE;

	for (int32 i = 0; i < OwnedAugments.Num(); i++)
	{
		if (OwnedAugments[i].AugmentID == AugmentID)
		{
			FoundIndex = i;
			break;
		}
	}

	if (FoundIndex != INDEX_NONE)
	{
		LevelUpAugment(FoundIndex);
	}
	else
	{
		AddAugment(AugmentID);
	}

	ApplyAugmentToASC(AugmentID);

	OnRep_OwnedAugments();
}

void AGRPlayerState::AddAugment(FName AugmentID)
{
	FAugmentEntry NewEntry;
	NewEntry.AugmentID = AugmentID;
	NewEntry.Level = 1;

	OwnedAugments.Add(NewEntry);
}

void AGRPlayerState::LevelUpAugment(int32 Index)
{
	if (!OwnedAugments.IsValidIndex(Index))
	{
		return;
	}

	OwnedAugments[Index].Level++;
}

void AGRPlayerState::ApplyAugmentToASC(FName AugmentID)
{
	if (!HasAuthority())
	{
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}
	
	UGRAugmentSubsystem* AugmentSubsystem = GetGameInstance()->GetSubsystem<UGRAugmentSubsystem>();
	if (!AugmentSubsystem)
	{
		return;
	}

	UGRAugmentDefinition* AugmentDef = AugmentSubsystem->GetAugment(AugmentID);
	if (!AugmentDef)
	{
		return;
	}

	if (FActiveGameplayEffectHandle* ExistingHandle = ActiveAugmentEffectHandles.Find(AugmentID))
	{
		ASC->RemoveActiveGameplayEffect(*ExistingHandle);
		ActiveAugmentEffectHandles.Remove(AugmentID);
	}
	
	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(AugmentDef->AugmentGameplayEffect,1.f, ASC->MakeEffectContext());
	if (!SpecHandle.IsValid())
	{
		return;
	}
	
	const int32 CurrentAugmentLevel = GetAugmentLevel(AugmentID);
	if (CurrentAugmentLevel <= 0)
	{
		return;
	}
	
	InitAugmentDefaults(SpecHandle, AugmentDef);

	ApplyAugmentValues(SpecHandle, AugmentDef, CurrentAugmentLevel);

	if (CurrentAugmentLevel >= 3)
	{
		ApplyAdditionalAugmentValues(SpecHandle, AugmentDef);
	}
	
	FActiveGameplayEffectHandle NewHandle = ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
	if (NewHandle.IsValid())
	{
		ActiveAugmentEffectHandles.Add(AugmentID, NewHandle);
	}
}

float AGRPlayerState::CalculateFinalAugmentValue(float Value, EAugmentModifierOpType Op)
{
	if (Op == EAugmentModifierOpType::Add)
	{
		return Value;
	}
	else if (Op == EAugmentModifierOpType::Subtract)
	{
		return -Value;
	}
	else if (Op == EAugmentModifierOpType::Multiply)
	{
		return FMath::Clamp(1.0f + Value , 0.0f, 10.0f);
	}
	else if (Op == EAugmentModifierOpType::InverseMultiply)
	{
		return FMath::Clamp(1.0f - Value , 0.0f, 10.0f);
	}
	else
	{
		return 0.0f;
	}
}

void AGRPlayerState::ApplyAugmentValues(FGameplayEffectSpecHandle& SpecHandle, const UGRAugmentDefinition* AugmentDef,
	int32 CurrentAugmentLevel)
{
	const int32 LevelIndex = CurrentAugmentLevel - 1;

	for (const FAugmentValues& Value : AugmentDef->AugmentValues)
	{
		if (!Value.ValuePerLevel.IsValidIndex(LevelIndex))
		{
			continue;
		}

		const float FinalValue = CalculateFinalAugmentValue(Value.ValuePerLevel[LevelIndex], Value.AugmentModifierOp);

		if (FinalValue != 0.0f)
		{
			SpecHandle.Data->SetSetByCallerMagnitude(Value.AugmentTag, FinalValue);
		}
	}
}

void AGRPlayerState::ApplyAdditionalAugmentValues(FGameplayEffectSpecHandle& SpecHandle,
	const UGRAugmentDefinition* AugmentDef)
{
	for (const FAugmentAdditionalValue& Additional : AugmentDef->AugmentAdditionalValues)
	{
		const float FinalValue = CalculateFinalAugmentValue(Additional.AugmentAdditionalValue, Additional.AugmentAdditionalModifierOp);

		if (FinalValue != 0.0f)
		{
			SpecHandle.Data->SetSetByCallerMagnitude(Additional.AugmentAdditionalTag, FinalValue);
		}
	}
}

void AGRPlayerState::InitAugmentDefaults(FGameplayEffectSpecHandle& SpecHandle, const UGRAugmentDefinition* AugmentDef)
{
	for (const FAugmentValues& Value : AugmentDef->AugmentValues)
	{
		float DefaultValue = 0.0f;

		if (Value.AugmentModifierOp == EAugmentModifierOpType::Multiply || Value.AugmentModifierOp == EAugmentModifierOpType::InverseMultiply)
		{
			DefaultValue = 1.0f;
		}

		SpecHandle.Data->SetSetByCallerMagnitude(Value.AugmentTag, DefaultValue);
	}

	for (const FAugmentAdditionalValue& Additional : AugmentDef->AugmentAdditionalValues)
	{
		float DefaultValue = 0.0f;

		if (Additional.AugmentAdditionalModifierOp == EAugmentModifierOpType::Multiply || Additional.AugmentAdditionalModifierOp == EAugmentModifierOpType::InverseMultiply)
		{
			DefaultValue = 1.0f;
		}

		SpecHandle.Data->SetSetByCallerMagnitude(Additional.AugmentAdditionalTag, DefaultValue);
	}
}

int32 AGRPlayerState::GetAugmentLevel(FName AugmentID)
{
	for (const FAugmentEntry& Entry : OwnedAugments)
	{
		if (Entry.AugmentID == AugmentID)
		{
			return Entry.Level;
		}
	}

	return 0;
}

void AGRPlayerState::OnRep_OwnedAugments()
{
	AGRBattlePlayerController* BattlePlayerController = GetOwner<AGRBattlePlayerController>();
	if (!IsValid(BattlePlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("GetOwner() is NOT AGRBattlePlayerController"));
		return;
	}

	BattlePlayerController->HideAugmentWidget();
}
