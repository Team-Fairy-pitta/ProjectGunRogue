#include "Character/GRCharacter.h"
#include "Player/Battle/GRBattlePlayerController.h"
#include "Player/GRPlayerState.h"
#include "GameModes/GRGameState.h"
#include "AbilitySystem/GRGameplayEffect.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/GRHealthAttributeSet.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"
#include "Weapon/GRWeaponDefinition.h"
#include "UI/BattleHUD/GRBattleHUDWidget.h"
#include "UI/BattleHUD/GRSpectatorHUDWidget.h"
#include "UI/BattleHUD/GRGameOverWidget.h"
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

	if (Data.NewValue < Data.OldValue)
	{
		if (HealthHitEffectWidget)
		{
			UUserWidget* HPHitWidget = CreateWidget<UUserWidget>(this,HealthHitEffectWidget);
			if (HPHitWidget)
			{
				HPHitWidget->AddToViewport();
			}
		}
	}
}

void AGRBattlePlayerController::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	UpdatePlayerMaxHealth(Data.NewValue);
}

void AGRBattlePlayerController::OnShieldChanged(const FOnAttributeChangeData& Data)
{
	UpdatePlayerShield(Data.NewValue);

	if (Data.NewValue < Data.OldValue)
	{
		if (ShieldHitEffectWidget)
		{
			UUserWidget* ShieldHitWidget = CreateWidget<UUserWidget>(this,ShieldHitEffectWidget);
			if (ShieldHitWidget)
			{
				ShieldHitWidget->AddToViewport();
			}
		}
	}
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

void AGRBattlePlayerController::ShowSpectatorHUD()
{
	if (!SpectatorWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("SpectatorWidgetInstance is INVALID"));
		return;
	}
	if (!SpectatorWidgetInstance->IsInViewport())
	{
		SpectatorWidgetInstance->AddToViewport();
	}

	FInputModeGameOnly Mode;
	SetInputMode(Mode);
	bShowMouseCursor = false;
}

void AGRBattlePlayerController::HideSpectatorHUD()
{
	if (!SpectatorWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("SpectatorWidgetInstance is INVALID"));
		return;
	}
	if (SpectatorWidgetInstance->IsInViewport())
	{
		SpectatorWidgetInstance->RemoveFromParent();
	}
}

void AGRBattlePlayerController::BindSpectatorInput()
{
	// NOTE: 관전 상태에서의 키 입력을 하드 바인딩
	if (InputComponent)
	{
		InputComponent->BindKey(EKeys::PageUp, IE_Pressed, this, &ThisClass::ServerRPC_SpectatePreviousPlayer);
		InputComponent->BindKey(EKeys::PageDown, IE_Pressed, this, &ThisClass::ServerRPC_SpectateNextPlayer);
	}
}

AActor* AGRBattlePlayerController::GetPreviousSpectateActor()
{
	TArray<AActor*> AlivePlayerList = GetAlivePlayerList();
	if (AlivePlayerList.IsValidIndex(CurrentSpectateIndex))
	{
		CurrentSpectateIndex = CurrentSpectateIndex - 1 < 0 ? AlivePlayerList.Num() - 1 : CurrentSpectateIndex - 1;
		return AlivePlayerList[CurrentSpectateIndex];
	}
	else
	{
		if (AlivePlayerList.Num() > 0)
		{
			CurrentSpectateIndex = 0;
			return AlivePlayerList[CurrentSpectateIndex];
		}
		else
		{
			CurrentSpectateIndex = INDEX_NONE;
			return nullptr;
		}
	}
}

AActor* AGRBattlePlayerController::GetNextSpectateActor()
{
	TArray<AActor*> AlivePlayerList = GetAlivePlayerList();
	if (AlivePlayerList.IsValidIndex(CurrentSpectateIndex))
	{
		CurrentSpectateIndex = CurrentSpectateIndex + 1 >= AlivePlayerList.Num() ? 0 : CurrentSpectateIndex + 1;
		return AlivePlayerList[CurrentSpectateIndex];
	}
	else
	{
		if (AlivePlayerList.Num() > 0)
		{
			CurrentSpectateIndex = 0;
			return AlivePlayerList[CurrentSpectateIndex];
		}
		else
		{
			CurrentSpectateIndex = INDEX_NONE;
			return nullptr;
		}
	}
}

TArray<AActor*> AGRBattlePlayerController::GetAlivePlayerList()
{
	if (!GetWorld())
	{
		return TArray<AActor*>();
	}

	AGameStateBase* GameStateBsae = GetWorld()->GetGameState<AGameStateBase>();
	if (!IsValid(GameStateBsae))
	{
		return TArray<AActor*>();
	}

	TArray<AActor*> PlayerList;

	for (APlayerState* ItState : GameStateBsae->PlayerArray)
	{
		AGRPlayerState* GRPlayerState = Cast<AGRPlayerState>(ItState);
		if (IsValid(GRPlayerState) && !GRPlayerState->IsDead())
		{
			APawn* ItPawn = GRPlayerState->GetPawn();
			if (IsValid(ItPawn))
			{
				PlayerList.Add(ItPawn);
			}
		}
	}

	return PlayerList;
}

void AGRBattlePlayerController::ClientRPC_SetSpectationTargetPlayerName_Implementation(AActor* Target)
{
	if (!IsValid(Target))
	{
		return;
	}

	ACharacter* TargetCharacter = Cast<ACharacter>(Target);
	if (IsValid(TargetCharacter))
	{
		if (TargetCharacter->GetPlayerState())
		{
			const FString& PlayerName = TargetCharacter->GetPlayerState()->GetPlayerName();
			
			if (SpectatorWidgetInstance)
			{
				SpectatorWidgetInstance->SetTargetPlayerName(PlayerName);
			}
		}
	}
}

void AGRBattlePlayerController::ServerRPC_SpectatePreviousPlayer_Implementation()
{
	if (GetStateName() != NAME_Spectating)
	{
		return;
	}

	AActor* TargetActor = GetPreviousSpectateActor();
	SetSpectatePlayer(TargetActor);
}

void AGRBattlePlayerController::ServerRPC_SpectateNextPlayer_Implementation()
{
	if (GetStateName() != NAME_Spectating)
	{
		return;
	}

	AActor* TargetActor = GetNextSpectateActor();
	SetSpectatePlayer(TargetActor);
}

void AGRBattlePlayerController::SetSpectatePlayer(AActor* TargetPlayer)
{
	AGRCharacter* GRCharacter = Cast<AGRCharacter>(TargetPlayer);
	if (!IsValid(GRCharacter))
	{
		ChangeState(NAME_Playing);
		return;
	}

	AGRPlayerState* GRPlayerState = GRCharacter->GetGRPlayerState();
	if (!IsValid(GRPlayerState))
	{
		ChangeState(NAME_Playing);
		return;
	}

	ChangeState(NAME_Spectating);
	ClientRPC_SetSpectationTargetPlayerName(GRCharacter);
	SetViewTargetWithBlend(GRCharacter);
}

void AGRBattlePlayerController::ServerRPC_StartSpectating_Implementation()
{
	if (!HasAuthority())
	{
		return;
	}

	ChangeState(NAME_Spectating);
	ServerRPC_SpectateNextPlayer();
}

void AGRBattlePlayerController::ClientRPC_StartSpectating_Implementation()
{
	HideBattleHUD();
	HideInventoryWidget();
	HideInGameMenuWidget();
	HideAugmentWidget();
	HideLevel1SelectWidget();
	HideUpgradeConsoleWidget();

	ShowSpectatorHUD();
}

void AGRBattlePlayerController::ServerRPC_GameOver_Implementation()
{
	if (!HasAuthority())
	{
		return;
	}

	if (!GetWorld())
	{
		return;
	}

	if (LobbyMap.IsNull())
	{
		UE_LOG(LogTemp, Error, TEXT("LobbyMap is NULL"));
		return;
	}

	FString MapPath = LobbyMap.GetLongPackageName() + TEXT("?listen");
	GetWorld()->ServerTravel(MapPath);
}

void AGRBattlePlayerController::ClientRPC_GameOver_Implementation()
{
	HideAugmentWidget();
	HideBattleHUD();
	HideInGameMenuWidget();
	HideInventoryWidget();
	HideLevel1SelectWidget();
	HideSpectatorHUD();
	
	ShowGameOverWidget();
}

void AGRBattlePlayerController::ShowGameOverWidget()
{
	if (!GameOverWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("GameOverWidgetInstance is INVALID"));
		return;
	}
	if (!GameOverWidgetInstance->IsInViewport())
	{
		GameOverWidgetInstance->AddToViewport();
	}

	FInputModeUIOnly Mode;
	SetInputMode(Mode);
	bShowMouseCursor = true;
}

void AGRBattlePlayerController::HideGameOverWidget()
{
	if (!GameOverWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("GameOverWidgetInstance is INVALID"));
		return;
	}
	if (GameOverWidgetInstance->IsInViewport())
	{
		GameOverWidgetInstance->RemoveFromParent();
	}

	FInputModeGameOnly Mode;
	SetInputMode(Mode);
	bShowMouseCursor = false;
}
