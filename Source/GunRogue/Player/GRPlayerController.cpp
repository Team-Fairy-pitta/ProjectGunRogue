#include "Player/GRPlayerController.h"
#include "Player/GRPlayerState.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/GRGameplayEffect.h"

void AGRPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	AGRPlayerState* GRPlayerState = GetPlayerState<AGRPlayerState>();
	if (!IsValid(GRPlayerState))
	{
		return;
	}

	UGRAbilitySystemComponent* ASC = GRPlayerState->GetGRAbilitySystemComponent();
	ASC->ProcessAbilityInput(DeltaTime, bGamePaused);
}

void AGRPlayerController::ClientRPC_OnActiveGameplayEffectAdded_Implementation(TSubclassOf<UGameplayEffect> EffectClass)
{
	if (!IsLocalController())
	{
		return;
	}

	// TODO: HUD와 연동해서, 체력바 위에 현재 활성화된 GameplayEffect 아이콘을 추가해야 함
	if (GEngine)
	{
		UGRGameplayEffect* EffectCDO = EffectClass->GetDefaultObject<UGRGameplayEffect>();
		if (EffectCDO)
		{
			FString EffectIconName = EffectCDO->EffectIcon.GetFullName();
			FString EffectName = EffectCDO->EffectName.ToString();
			FString EffectDescription = EffectCDO->EffectDescription.ToString();
			FString DebugMessage = FString::Printf(TEXT("Added: %s / %s / %s"), *EffectIconName, *EffectName, *EffectDescription);
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Cyan, DebugMessage);
		}
	}
}

void AGRPlayerController::ClientRPC_OnActiveGameplayEffectRemoved_Implementation(TSubclassOf<UGameplayEffect> EffectClass)
{
	if (!IsLocalController())
	{
		return;
	}

	// TODO: HUD와 연동해서, 체력바 위에 현재 활성화된 GameplayEffect 아이콘을 제거해야 함
	if (GEngine)
	{
		UGRGameplayEffect* EffectCDO = EffectClass->GetDefaultObject<UGRGameplayEffect>();
		if (EffectCDO)
		{
			FString EffectIconName = EffectCDO->EffectIcon.GetFullName();
			FString EffectName = EffectCDO->EffectName.ToString();
			FString EffectDescription = EffectCDO->EffectDescription.ToString();
			FString DebugMessage = FString::Printf(TEXT("Removed: %s / %s / %s"), *EffectIconName, *EffectName, *EffectDescription);
			GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Cyan, DebugMessage);
		}
	}
}
