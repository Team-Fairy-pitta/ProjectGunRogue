// GRGoodsAttributeSet.cpp


#include "AbilitySystem/Attributes/GRGoodsAttributeSet.h"
#include "Player/GRPlayerState.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UGRGoodsAttributeSet::UGRGoodsAttributeSet()
{
	InitGoldMultiplier(1.f);
	InitGemMultiplier(1.f);
	
	InitGainGold(0.f);
	InitGainGem(0.f);
}

void UGRGoodsAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGRGoodsAttributeSet, GoldMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRGoodsAttributeSet, GemMultiplier, COND_None, REPNOTIFY_Always);
}

void UGRGoodsAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	UAbilitySystemComponent* OwningASC = GetOwningAbilitySystemComponent();
	if (!OwningASC || OwningASC->GetOwnerRole() != ROLE_Authority)
	{
		return;
	}

	APawn* OwnerPawn = Cast<APawn>(OwningASC->GetAvatarActor());
	if (!OwnerPawn)
	{
		return;
	}

	AGRPlayerState* PS = OwnerPawn->GetPlayerState<AGRPlayerState>();
	if (!PS)
	{
		return;
	}

	if (Data.EvaluatedData.Attribute == GetGainGoldAttribute())
	{
		float BaseGold = GetGainGold();
		SetGainGold(0.f);

		if (BaseGold > 0.f)
		{
			const float Multiplier = GetGoldMultiplier();

			UE_LOG(LogTemp, Warning,
				TEXT("[Gold] Base=%f Multiplier=%f Final=%f"),
				BaseGold,
				Multiplier,
				BaseGold * Multiplier
			);

			float FinalGold = BaseGold * Multiplier;

			PS->ApplyGoldGain(FMath::RoundToInt(FinalGold));
		}
	}

	if (Data.EvaluatedData.Attribute == GetGainGemAttribute())
	{
		float BaseGem = GetGainGem();
		SetGainGem(0.f);

		if (BaseGem > 0.f)
		{
			const float Multiplier = GetGemMultiplier();

			UE_LOG(LogTemp, Warning,
				TEXT("[Gem] Base=%f Multiplier=%f Final=%f"),
				BaseGem,
				Multiplier,
				BaseGem * Multiplier
			);

			float FinalGem = BaseGem * Multiplier;

			PS->ApplyMetaGoodsGain(FMath::RoundToInt(FinalGem));
		}
	}
}

void UGRGoodsAttributeSet::OnRep_GoldMultiplier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRGoodsAttributeSet, GoldMultiplier, OldValue);
}

void UGRGoodsAttributeSet::OnRep_GemMultiplier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRGoodsAttributeSet, GemMultiplier, OldValue);
}
