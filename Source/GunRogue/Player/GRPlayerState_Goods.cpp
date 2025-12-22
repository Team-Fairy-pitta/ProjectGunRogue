#include "Battle/GRBattlePlayerController.h"
#include "Player/GRPlayerState.h"

#pragma region Goods
void AGRPlayerState::AddMetaGoods(int32 Amount)
{
	if (!HasAuthority())
	{
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC || !GainGemGE)
	{
		return;
	}

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GainGemGE, 1.f, ASC->MakeEffectContext());
	if (!SpecHandle.IsValid())
	{
		return;
	}

	FGameplayTag GemTag = FGameplayTag::RequestGameplayTag(TEXT("Attribute.Data.GainGem"));
	
	SpecHandle.Data->SetSetByCallerMagnitude(GemTag, static_cast<float>(Amount));
	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void AGRPlayerState::AddGold(int32 Amount)
{
	if (!HasAuthority())
	{
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC || !GainGoldGE)
	{
		return;
	}

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GainGoldGE, 1.f, ASC->MakeEffectContext());
	if (!SpecHandle.IsValid())
	{
		return;
	}

	FGameplayTag GoldTag = FGameplayTag::RequestGameplayTag(TEXT("Attribute.Data.GainGold"));
	
	SpecHandle.Data->SetSetByCallerMagnitude(GoldTag, static_cast<float>(Amount));
	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void AGRPlayerState::AddHealthByHealthKit(int32 Amount)
{
	if (!HasAuthority())
	{
		return;
	}

	UAbilitySystemComponent* ASC = GetAbilitySystemComponent();
	if (!ASC || !GainHealthKitGE)
	{
		return;
	}

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(GainHealthKitGE, 1.f, ASC->MakeEffectContext());
	if (!SpecHandle.IsValid())
	{
		return;
	}

	FGameplayTag HealthTag = FGameplayTag::RequestGameplayTag(TEXT("Attribute.Data.GainHealing"));
	
	SpecHandle.Data->SetSetByCallerMagnitude(HealthTag, static_cast<float>(Amount));
	ASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
}

void AGRPlayerState::ApplyMetaGoodsGain(int32 Amount)
{
	if (!HasAuthority())
	{
		return;
	}

	CurrentMetaGoods += Amount;
	OnRep_CurrentMetaGoods();
}

void AGRPlayerState::ApplyGoldGain(int32 Amount)
{
	if (!HasAuthority())
	{
		return;
	}

	Gold += Amount;
	OnRep_Gold();
}

void AGRPlayerState::OnRep_CurrentMetaGoods()
{
	if (GetPawn() && GetPawn()->IsLocallyControlled())
	{
		UpdateMetaGoodsUI();
		SavePerkToSave();
	}
}

void AGRPlayerState::OnRep_Gold()
{
	if (GetPawn() && GetPawn()->IsLocallyControlled())
	{
		UpdateGoldUI();
	}
}

void AGRPlayerState::UpdateMetaGoodsUI()
{
	AGRBattlePlayerController* BattlePlayerController = GetOwner<AGRBattlePlayerController>();
	if (!IsValid(BattlePlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("GetOwner() is NOT AGRBattlePlayerController"));
		return;
	}

	BattlePlayerController->SyncMetaGoodsUI();
}

void AGRPlayerState::UpdateGoldUI()
{
	AGRBattlePlayerController* BattlePlayerController = GetOwner<AGRBattlePlayerController>();
	if (!IsValid(BattlePlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("GetOwner() is NOT AGRBattlePlayerController"));
		return;
	}

	BattlePlayerController->SyncGoldUI();
}
#pragma endregion

