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

	if (!Augment_BombGE)
	{
		return;
	}

	UGRAugmentSubsystem* AugmentSubsystem = GetGameInstance()->GetSubsystem<UGRAugmentSubsystem>();
	if (!AugmentSubsystem)
	{
		return;
	}

	UGRAugmentDefinition* CurrentAugmentDefinition = AugmentSubsystem->GetAugment(AugmentID);
	if (!CurrentAugmentDefinition)
	{
		return;
	}

	FGameplayTag AugmentRootTag = FGameplayTag::RequestGameplayTag(FName("Augment"));
	ASC->RemoveActiveEffectsWithGrantedTags(FGameplayTagContainer(AugmentRootTag));

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(Augment_BombGE,1.f, ASC->MakeEffectContext());
	if (!SpecHandle.IsValid())
	{
		return;
	}

	SpecHandle.Data->DynamicGrantedTags.AddTag(AugmentRootTag);

	const int32 CurrentAugmentLevel = GetAugmentLevel(AugmentID);
	if (CurrentAugmentLevel <= 0)
	{
		return;
	}
	
	const int32 LevelIndex = CurrentAugmentLevel - 1;
	
	for (const FAugmentValues& Value : CurrentAugmentDefinition->AugmentValues)
	{
		if (!Value.ValuePerLevel.IsValidIndex(LevelIndex))
		{
			continue;
		}

		EAugmentModifierOpType AugmentModifierOp = Value.AugmentModifierOp;

		float LevelValue = Value.ValuePerLevel[LevelIndex];
		float FinalValue = 0.0f;

		if (AugmentModifierOp == EAugmentModifierOpType::Add)
		{
			FinalValue = LevelValue;
		}
		else if (AugmentModifierOp == EAugmentModifierOpType::Subtract)
		{
			FinalValue = -LevelValue;
		}
		else if (AugmentModifierOp == EAugmentModifierOpType::Multiply)
		{
			FinalValue = FMath::Clamp(1.0f + LevelValue, 0.0f, 10.0f);
		}
		else if (AugmentModifierOp == EAugmentModifierOpType::InverseMultiply)
		{
			FinalValue = FMath::Clamp(1.0f - LevelValue, 0.0f, 10.0f);
		}
		else
		{
			continue;
		}
		
		SpecHandle.Data->SetSetByCallerMagnitude(Value.AugmentTag, FinalValue);
	}

	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
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
	for (const FAugmentEntry& Entry : OwnedAugments)
	{
		const FAugmentEntry* PrevEntry = nullptr;
		for (const FAugmentEntry& Prev : PreviousOwnedAugments)
		{
			if (Prev.AugmentID == Entry.AugmentID)
			{
				PrevEntry = &Prev;
				break;
			}
		}

		if (!PrevEntry || PrevEntry->Level != Entry.Level)
		{
			OnAugmentChanged.Broadcast(Entry.AugmentID, Entry.Level);
			UE_LOG(LogTemp, Warning, TEXT("OnRep_OwnedAugments called"));
		}
	}

	PreviousOwnedAugments = OwnedAugments;

	AGRBattlePlayerController* BattlePlayerController = GetOwner<AGRBattlePlayerController>();
	if (!IsValid(BattlePlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("GetOwner() is NOT AGRBattlePlayerController"));
		return;
	}

	BattlePlayerController->HideAugmentWidget();
}
