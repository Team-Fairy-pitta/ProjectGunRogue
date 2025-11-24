#include "Player/Battle/GRBattlePlayerController.h"
#include "Player/GRPlayerState.h"
#include "GameModes/GRGameState.h"
#include "AbilitySystem/GRGameplayEffect.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/GRHealthAttributeSet.h"
#include "UI/BattleHUD/GRBattleHUDWidget.h"
#include "UI/BattleHUD/SubWidgets/GRPlayerStatusWidget.h"
#include "UI/BattleHUD/SubWidgets/GRTeamStatusListWidget.h"
#include "UI/BattleHUD/SubWidgets/GRTeamStatusWidget.h"

void AGRBattlePlayerController::UpdatePlayerHealth(float Value)
{
	if (!HUDWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("HUDWidgetInstance (UGRBattleHUDWidget) is INVALID"));
		return;
	}

	UGRPlayerStatusWidget* PlayerStatusWidget = HUDWidgetInstance->GetPlayerStatusWidget();
	if (!PlayerStatusWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerStatusWidget (UGRPlayerStatusWidget) is INVALID"));
		return;
	}

	PlayerStatusWidget->SetPlayerHealth(Value);
}

void AGRBattlePlayerController::UpdatePlayerMaxHealth(float Value)
{
	if (!HUDWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("HUDWidgetInstance (UGRBattleHUDWidget) is INVALID"));
		return;
	}

	UGRPlayerStatusWidget* PlayerStatusWidget = HUDWidgetInstance->GetPlayerStatusWidget();
	if (!PlayerStatusWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerStatusWidget (UGRPlayerStatusWidget) is INVALID"));
		return;
	}

	PlayerStatusWidget->SetPlayerMaxHealth(Value);
}

void AGRBattlePlayerController::UpdatePlayerShield(float Value)
{
	if (!HUDWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("HUDWidgetInstance (UGRBattleHUDWidget) is INVALID"));
		return;
	}

	UGRPlayerStatusWidget* PlayerStatusWidget = HUDWidgetInstance->GetPlayerStatusWidget();
	if (!PlayerStatusWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerStatusWidget (UGRPlayerStatusWidget) is INVALID"));
		return;
	}

	PlayerStatusWidget->SetPlayerShield(Value);
}

void AGRBattlePlayerController::UpdatePlayerMaxShield(float Value)
{
	if (!HUDWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("HUDWidgetInstance (UGRBattleHUDWidget) is INVALID"));
		return;
	}

	UGRPlayerStatusWidget* PlayerStatusWidget = HUDWidgetInstance->GetPlayerStatusWidget();
	if (!PlayerStatusWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerStatusWidget (UGRPlayerStatusWidget) is INVALID"));
		return;
	}

	PlayerStatusWidget->SetPlayerMaxShield(Value);
}

void AGRBattlePlayerController::OnUpdateOtherPlayerStatus()
{
	if (!HUDWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("HUDWidgetInstance (UGRBattleHUDWidget) is INVALID"));
		return;
	}

	UGRTeamStatusListWidget* TeamStatusWidget = HUDWidgetInstance->GetTeamStatusListWidget();
	if (!TeamStatusWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("TeamStatusWidget (UGRTeamStatusListWidget) is INVALID"));
		return;
	}

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		UE_LOG(LogTemp, Error, TEXT("GetWorld() is INVALID"));
		return;
	}

	AGRGameState* GRGameState = World->GetGameState<AGRGameState>();
	if (!IsValid(GRGameState))
	{
		UE_LOG(LogTemp, Error, TEXT("GRGameState (AGRGameState) is INVALID"));
		return;
	}

	AGRPlayerState* GRPlayerState = GetPlayerState<AGRPlayerState>();
	if (!IsValid(GRPlayerState))
	{
		UE_LOG(LogTemp, Error, TEXT("GRPlayerState (AGRPlayerState) is INVALID"));
		return;
	}

	int32 OtherPlayerCount = GRGameState->PlayerArray.Num() - 1;
	while (TeamStatusWidget->GetTeamStatusWidgetCount() < OtherPlayerCount)
	{
		TeamStatusWidget->CreateTeamStatus();
	}
	while (TeamStatusWidget->GetTeamStatusWidgetCount() > OtherPlayerCount)
	{
		TeamStatusWidget->DestroyLastTeamStatus();
	}

	int32 PlayerIndex = 0;
	for (APlayerState* OtherPlayerState : GRGameState->PlayerArray)
	{
		AGRPlayerState* OtherGRPlayerState = Cast<AGRPlayerState>(OtherPlayerState);
		if (!IsValid(OtherGRPlayerState))
		{
			UE_LOG(LogTemp, Warning, TEXT("OtherGRPlayerState (AGRPlayerState) is INVALID"));
		}

		if (OtherGRPlayerState == GRPlayerState)
		{
			continue;
		}

		UGRAbilitySystemComponent* ASC = OtherGRPlayerState->GetGRAbilitySystemComponent();
		if (!IsValid(ASC))
		{
			UE_LOG(LogTemp, Error, TEXT("ASC (UGRAbilitySystemComponent) is INVALID"));
			return;
		}

		const UAttributeSet* AttributeSet = ASC->GetAttributeSet(UGRHealthAttributeSet::StaticClass());
		const UGRHealthAttributeSet* HealthSet = Cast<UGRHealthAttributeSet>(AttributeSet);
		if (!IsValid(HealthSet))
		{
			UE_LOG(LogTemp, Error, TEXT("HealthSet (UGRHealthAttributeSet) is INVALID"));
			return;
		}

		float Health = HealthSet->GetHealth();
		float MaxHealth = HealthSet->GetMaxHealth();
		float Shield = HealthSet->GetShield();
		float MaxShield = HealthSet->GetMaxShield();

		TeamStatusWidget->SetTeamHPBar(PlayerIndex, Health, MaxHealth);
		TeamStatusWidget->SetTeamShieldBar(PlayerIndex, Shield, MaxShield);
		PlayerIndex += 1;
	}
}

void AGRBattlePlayerController::ClientRPC_OnActiveGameplayEffectAdded_Implementation(TSubclassOf<UGameplayEffect> EffectClass)
{
	if (!IsLocalController())
	{
		return;
	}

	if (!HUDWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("HUDWidgetInstance (UGRBattleHUDWidget) is INVALID"));
		return;
	}

	UGRPlayerStatusWidget* PlayerStatusWidget = HUDWidgetInstance->GetPlayerStatusWidget();
	if (!PlayerStatusWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerStatusWidget (UGRPlayerStatusWidget) is INVALID"));
		return;
	}

	PlayerStatusWidget->AddBuffIcon(EffectClass);
}

void AGRBattlePlayerController::ClientRPC_OnActiveGameplayEffectRemoved_Implementation(TSubclassOf<UGameplayEffect> EffectClass)
{
	if (!IsLocalController())
	{
		return;
	}

	if (!HUDWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("HUDWidgetInstance (UGRBattleHUDWidget) is INVALID"));
		return;
	}

	UGRPlayerStatusWidget* PlayerStatusWidget = HUDWidgetInstance->GetPlayerStatusWidget();
	if (!PlayerStatusWidget)
	{
		UE_LOG(LogTemp, Error, TEXT("PlayerStatusWidget (UGRPlayerStatusWidget) is INVALID"));
		return;
	}

	PlayerStatusWidget->RemoveBuffIcon(EffectClass);
}
