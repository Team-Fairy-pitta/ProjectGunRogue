#include "Player/Battle/GRBattlePlayerController.h"
#include "Player/GRPlayerState.h"
#include "GameModes/GRGameState.h"
#include "AbilitySystem/GRGameplayEffect.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/GRHealthAttributeSet.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"
#include "Weapon/GRWeaponDefinition.h"
#include "UI/BattleHUD/GRBattleHUDWidget.h"
#include "UI/BattleHUD/SubWidgets/GRWeaponListWidget.h"
#include "UI/BattleHUD/SubWidgets/GRPlayerStatusWidget.h"
#include "UI/BattleHUD/SubWidgets/GRTeamStatusListWidget.h"
#include "UI/BattleHUD/SubWidgets/GRTeamStatusWidget.h"
#include "UI/Damage/GRDamageIndicator.h"
#include "MiniMap/GRRadarMapComponent.h"

void AGRBattlePlayerController::InitializeBattleHUD()
{
	AGRPlayerState* GRPlayerState = GetPlayerState<AGRPlayerState>();
	if (!IsValid(GRPlayerState))
	{
		UE_LOG(LogTemp, Error, TEXT("GRPlayerState (AGRPlayerState) is INVALID"));
		return;
	}

	UGRAbilitySystemComponent* ASC = GRPlayerState->GetGRAbilitySystemComponent();
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

	ASC->GetGameplayAttributeValueChangeDelegate(
		UGRHealthAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);

	ASC->GetGameplayAttributeValueChangeDelegate(
		UGRHealthAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &ThisClass::OnMaxHealthChanged);

	ASC->GetGameplayAttributeValueChangeDelegate(
		UGRHealthAttributeSet::GetShieldAttribute()).AddUObject(this, &ThisClass::OnShieldChanged);

	ASC->GetGameplayAttributeValueChangeDelegate(
		UGRHealthAttributeSet::GetMaxShieldAttribute()).AddUObject(this, &ThisClass::OnMaxShieldChanged);


	float Health = HealthSet->GetHealth();
	float MaxHealth = HealthSet->GetMaxHealth();
	float Shield = HealthSet->GetShield();
	float MaxShield = HealthSet->GetMaxShield();

	UpdatePlayerHealth(Health);
	UpdatePlayerMaxHealth(MaxHealth);
	UpdatePlayerShield(Shield);
	UpdatePlayerMaxShield(MaxShield);
	
	SyncGoldUI();
	SyncMetaGoodsUI();

	GetWorldTimerManager().SetTimer(OtherPlayerStatusUpdateTimer, this, &ThisClass::OnUpdateOtherPlayerStatus, OtherPlayerStatusUpdateInterval, true);


	if (!GRPlayerState->OnWeaponEquipped.IsAlreadyBound(this, &ThisClass::OnWeaponEquipped))
	{
		GRPlayerState->OnWeaponEquipped.AddDynamic(this, &ThisClass::OnWeaponEquipped);
	}
	if (!GRPlayerState->OnWeaponDropped.IsAlreadyBound(this, &ThisClass::OnWeaponDropped))
	{
		GRPlayerState->OnWeaponDropped.AddDynamic(this, &ThisClass::OnWeaponDropped);
	}
	if (!GRPlayerState->OnWeaponSwitched.IsAlreadyBound(this, &ThisClass::OnWeaponSwitched))
	{
		GRPlayerState->OnWeaponSwitched.AddDynamic(this, &ThisClass::OnWeaponSwitched);
	}

	UGRCombatAttributeSet* CombatSet = const_cast<UGRCombatAttributeSet*>(ASC->GetSet<UGRCombatAttributeSet>());
	if (!CombatSet)
	{
		UE_LOG(LogTemp, Error, TEXT("CombatSet (UGRCombatAttributeSet) is INVALID"));
		return;
	}

	if (!CombatSet->OnAmmoChanged.IsAlreadyBound(this, &ThisClass::OnAmmoChanged))
	{
		CombatSet->OnAmmoChanged.AddDynamic(this, &ThisClass::OnAmmoChanged);
	}

	APawn* OwnerPawn = GetPawn();
	if (OwnerPawn)
	{
		UGRRadarMapComponent* RadarComponent = OwnerPawn->FindComponentByClass<UGRRadarMapComponent>();
		if (RadarComponent)
		{
			RadarComponent->InitRadarWidget();
		}
	}
}

void AGRBattlePlayerController::FinalizeBattleHUD()
{
	AGRPlayerState* GRPlayerState = GetPlayerState<AGRPlayerState>();
	if (!IsValid(GRPlayerState))
	{
		UE_LOG(LogTemp, Error, TEXT("GRPlayerState (AGRPlayerState) is INVALID"));
		return;
	}

	if (OtherPlayerStatusUpdateTimer.IsValid())
	{
		GetWorldTimerManager().ClearTimer(OtherPlayerStatusUpdateTimer);
	}

	if (GRPlayerState->OnWeaponEquipped.IsAlreadyBound(this, &ThisClass::OnWeaponEquipped))
	{
		GRPlayerState->OnWeaponEquipped.RemoveDynamic(this, &ThisClass::OnWeaponEquipped);
	}
	if (GRPlayerState->OnWeaponDropped.IsAlreadyBound(this, &ThisClass::OnWeaponDropped))
	{
		GRPlayerState->OnWeaponDropped.RemoveDynamic(this, &ThisClass::OnWeaponDropped);
	}
	if (GRPlayerState->OnWeaponSwitched.IsAlreadyBound(this, &ThisClass::OnWeaponSwitched))
	{
		GRPlayerState->OnWeaponSwitched.RemoveDynamic(this, &ThisClass::OnWeaponSwitched);
	}

	UGRAbilitySystemComponent* ASC = GRPlayerState->GetGRAbilitySystemComponent();
	if (!IsValid(ASC))
	{
		UE_LOG(LogTemp, Error, TEXT("ASC (UGRAbilitySystemComponent) is INVALID"));
		return;
	}

	UGRCombatAttributeSet* CombatSet = const_cast<UGRCombatAttributeSet*>(ASC->GetSet<UGRCombatAttributeSet>());
	if (!CombatSet)
	{
		UE_LOG(LogTemp, Error, TEXT("CombatSet (UGRCombatAttributeSet) is INVALID"));
		return;
	}

	if (CombatSet->OnAmmoChanged.IsAlreadyBound(this, &ThisClass::OnAmmoChanged))
	{
		CombatSet->OnAmmoChanged.RemoveDynamic(this, &ThisClass::OnAmmoChanged);
	}
}

void AGRBattlePlayerController::ShowBattleHUD()
{
	if (!HUDWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("HUDWidgetInstance is INVALID"));
		return;
	}
	if (!HUDWidgetInstance->IsInViewport())
	{
		HUDWidgetInstance->AddToViewport();
	}

	FInputModeGameOnly Mode;
	SetInputMode(Mode);
	bShowMouseCursor = false;
}

void AGRBattlePlayerController::HideBattleHUD()
{
	if (!HUDWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("HUDWidgetInstance is INVALID"));
		return;
	}
	if (HUDWidgetInstance->IsInViewport())
	{
		HUDWidgetInstance->RemoveFromParent();
	}
}

void AGRBattlePlayerController::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	UpdatePlayerHealth(Data.NewValue);
}

void AGRBattlePlayerController::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	UpdatePlayerMaxHealth(Data.NewValue);
}

void AGRBattlePlayerController::OnShieldChanged(const FOnAttributeChangeData& Data)
{
	UpdatePlayerShield(Data.NewValue);
}

void AGRBattlePlayerController::OnMaxShieldChanged(const FOnAttributeChangeData& Data)
{
	UpdatePlayerMaxShield(Data.NewValue);
}

void AGRBattlePlayerController::OnAmmoChanged(int32 CurrentAmmo, int32 MaxAmmo)
{
	if (!HUDWidgetInstance)
	{
		return;
	}

	UGRWeaponListWidget* WeaponListWidget = HUDWidgetInstance->GetWeaponListWidget();
	if (!WeaponListWidget)
	{
		return;
	}

	WeaponListWidget->UpdateBulletCount(CurrentAmmo, MaxAmmo);
}

void AGRBattlePlayerController::OnWeaponEquipped(int32 SlotIndex, UGRWeaponDefinition* WeaponDefinition)
{
	if (!HUDWidgetInstance)
	{
		return;
	}
	if (!WeaponDefinition)
	{
		return;
	}

	UGRWeaponListWidget* WeaponListWidget = HUDWidgetInstance->GetWeaponListWidget();
	if (!WeaponListWidget)
	{
		return;
	}

	WeaponListWidget->EnableWeaponSlot(SlotIndex);
	WeaponListWidget->UpdateWeaponImage(SlotIndex, WeaponDefinition->WeaponIcon);
}

void AGRBattlePlayerController::OnWeaponDropped(int32 SlotIndex, UGRWeaponDefinition* WeaponDefinition)
{
	if (!HUDWidgetInstance)
	{
		return;
	}

	UGRWeaponListWidget* WeaponListWidget = HUDWidgetInstance->GetWeaponListWidget();
	if (!WeaponListWidget)
	{
		return;
	}

	WeaponListWidget->DisableWeaponSlot(SlotIndex);
}

void AGRBattlePlayerController::OnWeaponSwitched(int32 OldSlotIndex, int32 NewSlotIndex)
{
	if (!HUDWidgetInstance)
	{
		return;
	}

	UGRWeaponListWidget* WeaponListWidget = HUDWidgetInstance->GetWeaponListWidget();
	if (!WeaponListWidget)
	{
		return;
	}

	WeaponListWidget->SetSelectedWeapon(NewSlotIndex);
}

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

	TArray<AGRPlayerState*> GRPlayerArray;
	for (APlayerState* OtherPlayerState : GRGameState->PlayerArray)
	{
		AGRPlayerState* OtherGRPlayerState = Cast<AGRPlayerState>(OtherPlayerState);
		if (IsValid(OtherGRPlayerState))
		{
			GRPlayerArray.Add(OtherGRPlayerState);
		}
	}

	int32 OtherPlayerCount = GRPlayerArray.Num() - 1;
	while (TeamStatusWidget->GetTeamStatusWidgetCount() < OtherPlayerCount)
	{
		TeamStatusWidget->CreateTeamStatus();
	}
	while (TeamStatusWidget->GetTeamStatusWidgetCount() > OtherPlayerCount)
	{
		TeamStatusWidget->DestroyLastTeamStatus();
	}

	int32 PlayerIndex = 0;
	for (AGRPlayerState* OtherGRPlayerState : GRPlayerArray)
	{
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

void AGRBattlePlayerController::ClientRPC_ShowDamageIndicator_Implementation(float Damage, AActor* DamagedActor)
{
	ShowDamageIndicator(Damage, DamagedActor);
}

void AGRBattlePlayerController::ShowDamageIndicator(float Damage, AActor* DamagedActor)
{
	if (!IsLocalController())
	{
		return;
	}

	if (!DamageIndicatorWidgetClass)
	{
		return;
	}

	DamageIndicatorWidgetInstance = CreateWidget<UGRDamageIndicator>(this, DamageIndicatorWidgetClass);
	if (!DamageIndicatorWidgetInstance)
	{
		return;
	}

	if (!IsValid(DamagedActor))
	{
		return;
	}

	DamageIndicatorWidgetInstance->SetData(Damage, DamagedActor);
	DamageIndicatorWidgetInstance->AddToViewport();
}
