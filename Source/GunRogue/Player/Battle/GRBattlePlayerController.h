#pragma once

#include "Player/GRPlayerController.h"
#include "GRBattlePlayerController.generated.h"

class UGRBattleHUDWidget;
class UGRLevel1SelectWidget;
class AGRLevel1ControlPanel;
class UGRWeaponUpgrade;
class UGRWeaponDefinition;
class UGRWeaponUpgradeWidgetSetting;
class UGRInventoryWidgetMain;
class UGRAugmentHUDWidget;
class UGRSpectatorHUDWidget;
class UGRDamageIndicator;
class UGRGameOverWidget;
class UGRRadarMapComponent;
class UGRInGameHUDWidget;
class AGRLuwoAICharacter;
struct FGameplayEffectSpec;
struct FOnAttributeChangeData;
struct FGRLevel1Data;


UCLASS()
class GUNROGUE_API AGRBattlePlayerController : public AGRPlayerController
{
	GENERATED_BODY()

public:
	AGRBattlePlayerController();
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual void SetupInputComponent() override;
	virtual void OnRep_PlayerState() override;

	UFUNCTION(Client, Reliable)
	void ClientRPC_OnRestartPlayer();

private:
	void CreateWidgets();
	void InitUISetup();

/* Head Up Display 관련 코드 */
#pragma region HUD
public:
	UFUNCTION(BlueprintCallable)
	void ShowBattleHUD();

	UFUNCTION(BlueprintCallable)
	void HideBattleHUD();

	// Duration이 있는 (무제한 포함) Effect가 추가 되었을 때 호출됨 (Instance는 호출되지 않음)
	UFUNCTION(Client, Reliable)
	void ClientRPC_OnActiveGameplayEffectAdded(TSubclassOf<UGameplayEffect> EffectClass);

	// Duration이 있는 (무제한 포함) Effect가 제거 되었을 때 호출됨 (Instance는 호출되지 않음)
	UFUNCTION(Client, Reliable)
	void ClientRPC_OnActiveGameplayEffectRemoved(TSubclassOf<UGameplayEffect> EffectClass);

	UFUNCTION(Client, Reliable)
	void ClientRPC_ShowDamageIndicator(float Damage, AActor* DamagedActor);

	UFUNCTION(Client, Reliable)
	void ClientRPC_ShowNotifyMessage(const FText& Message, float ShowMessageTime);

	UFUNCTION(BlueprintCallable, Server, Reliable)
	void ServerRPC_GameOver();

	UFUNCTION(Client, Reliable)
	void ClientRPC_GameOver();

	UFUNCTION(BlueprintCallable)
	void ShowGameOverWidget();

	UFUNCTION(BlueprintCallable)
	void HideGameOverWidget();

	UGRBattleHUDWidget* GetBattleHUDWidget() const { return HUDWidgetInstance; }

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UGRRadarMapComponent> RadarMapComponent;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunRogue|Map")
	TSoftObjectPtr<UWorld> LobbyMap;

	UPROPERTY()
	TObjectPtr<UGRBattleHUDWidget> HUDWidgetInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget|Class")
	TSubclassOf<UGRBattleHUDWidget> HUDWidgetClass;

	UPROPERTY()
	TObjectPtr<UGRDamageIndicator> DamageIndicatorWidgetInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget|Class")
	TSubclassOf<UGRDamageIndicator> DamageIndicatorWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget|Class")
	TSubclassOf<UUserWidget> HealthHitEffectWidget;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget|Class")
	TSubclassOf<UUserWidget> ShieldHitEffectWidget;

	UPROPERTY()
	TObjectPtr<UGRGameOverWidget> GameOverWidgetInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget|Class")
	TSubclassOf<UGRGameOverWidget> GameOverWidgetClass;

	FTimerHandle OtherPlayerStatusUpdateTimer;
	float OtherPlayerStatusUpdateInterval = 1.0f;

private:
	void InitializeBattleHUD();
	void FinalizeBattleHUD();

	void UpdatePlayerHealth(float Value);
	void UpdatePlayerMaxHealth(float Value);
	void UpdatePlayerShield(float Value);
	void UpdatePlayerMaxShield(float Value);
	
	void OnUpdateOtherPlayerStatus();

	void OnHealthChanged(const FOnAttributeChangeData& Data);
	void OnMaxHealthChanged(const FOnAttributeChangeData& Data);
	void OnShieldChanged(const FOnAttributeChangeData& Data);
	void OnMaxShieldChanged(const FOnAttributeChangeData& Data);

	UFUNCTION()
	void OnWeaponEquipped(int32 SlotIndex, UGRWeaponDefinition* WeaponDefinition);

	UFUNCTION()
	void OnWeaponDropped(int32 SlotIndex, UGRWeaponDefinition* WeaponDefinition);

	UFUNCTION()
	void OnWeaponSwitched(int32 OldSlotIndex, int32 NewSlotIndex);

	UFUNCTION()
	void OnAmmoChanged(int32 CurrentAmmo, int32 MaxAmmo);

	void ShowDamageIndicator(float Damage, AActor* DamagedActor);

	UFUNCTION()
	void ShowNotifyMessage(const FText& Message, float ShowMessageTime);

#pragma endregion HUD

/* 관전 관련 코드 */
#pragma region Spectator_HUD
public:
	UFUNCTION(BlueprintCallable)
	void ShowSpectatorHUD();

	UFUNCTION(BlueprintCallable)
	void HideSpectatorHUD();

	UFUNCTION(Server, Reliable)
	void ServerRPC_StartSpectating();

	UFUNCTION(Client, Reliable)
	void ClientRPC_StartSpectating();

	UFUNCTION(Server, Reliable)
	void ServerRPC_SpectatePreviousPlayer();

	UFUNCTION(Server, Reliable)
	void ServerRPC_SpectateNextPlayer();

	TArray<AActor*> GetAlivePlayerList();

protected:
	UPROPERTY()
	TObjectPtr<UGRSpectatorHUDWidget> SpectatorWidgetInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget|Class")
	TSubclassOf<UGRSpectatorHUDWidget> SpectatorWidgetClass;

	void BindSpectatorInput();

	void SetSpectatePlayer(AActor* TargetPlayer);
	AActor* GetPreviousSpectateActor();
	AActor* GetNextSpectateActor();

	UFUNCTION(Client, Reliable)
	void ClientRPC_SetSpectationTargetPlayerName(AActor* Target);

	int32 CurrentSpectateIndex = INDEX_NONE;

#pragma endregion

/* 무기 강화 UI (UpgradeConsole) 관련 코드 */
#pragma region UpgradeConsole
public:
	UFUNCTION(Client, Reliable)
	void ClientRPC_ShowUpgradeConsoleWidget();

	UFUNCTION(BlueprintCallable)
	void ShowUpgradeConsoleWidget();

	UFUNCTION(BlueprintCallable)
	void HideUpgradeConsoleWidget();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget|Class")
	TSubclassOf<UGRWeaponUpgradeWidgetSetting> UpgradeConsoleWidgetClass;

	UPROPERTY()
	TObjectPtr<UGRWeaponUpgradeWidgetSetting> UpgradeConsoleWidgetInstance;

#pragma endregion UpgradeConsole

/* 인벤토리 UI 관련 코드 */
#pragma region Inventory
public:
	UFUNCTION(Client, Reliable)
	void ClientRPC_ToggleInventoryWidget();

	UFUNCTION(BlueprintCallable)
	void ToggleInventoryWidget();

	UFUNCTION(BlueprintCallable)
	void ShowInventoryWidget();

	UFUNCTION(BlueprintCallable)
	void HideInventoryWidget();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget|Class")
	TSubclassOf<UGRInventoryWidgetMain> InventoryWidgetClass;

	UPROPERTY()
	TObjectPtr<UGRInventoryWidgetMain> InventoryWidgetInstance;
#pragma endregion Inventory

/* Level1 관련 코드 */
#pragma region Level1
public:
	UFUNCTION(Client, Reliable)
	void ClientRPC_ShowLevel1SelectWidget(AGRLevel1ControlPanel* ControlPanel);

	UFUNCTION(Client, Reliable)
	void ClientRPC_HideLevel1SelectWidget();

	UFUNCTION(Server, Reliable)
	void ServerRPC_OnSelectNextRoom(int32 NextRoomIndex, AGRLevel1ControlPanel* ControlPanel);

	UFUNCTION(BlueprintCallable)
	void ShowLevel1SelectWidget();

	UFUNCTION(BlueprintCallable)
	void HideLevel1SelectWidget();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget|Class")
	TSubclassOf<UGRLevel1SelectWidget> Level1SelectWidgetClass;

	UPROPERTY()
	TObjectPtr<UGRLevel1SelectWidget> Level1SelectWidgetInstance;

private:
	void SetLevel1SelectWidget(const FGRLevel1Data& Level1Data, AGRLevel1ControlPanel* ControlPanel);

#pragma endregion Level1

/* 증강 관련 코드 */
#pragma region Augment
public:
	UFUNCTION(Client, Reliable) 
	void ClientRPC_ShowAugmentWidget();
	
	UFUNCTION(BlueprintCallable)
	void ShowAugmentWidget();

	UFUNCTION(BlueprintCallable)
	void HideAugmentWidget();

	UFUNCTION()
	void RequestSelectAugment(FName AugmentID);
	
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget|Class")
	TSubclassOf<UGRAugmentHUDWidget> AugmentWidgetClass;

	UPROPERTY()
	TObjectPtr<UGRAugmentHUDWidget> AugmentWidgetInstance;
	
#pragma endregion Augment

/* 인게임 메뉴 코드 */
#pragma region Menu
public:
	UFUNCTION(Client, Reliable)
	void ClientRPC_ShowInGameMenuWidget();

	UFUNCTION(BlueprintCallable)
	void ShowInGameMenuWidget();

	UFUNCTION(BlueprintCallable)
	void HideInGameMenuWidget();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget|Class")
	TSubclassOf<UGRInGameHUDWidget> InGameMenuWidgetClass;

	UPROPERTY()
	TObjectPtr<UGRInGameHUDWidget> InGameMenuWidgetInstance;

#pragma endregion Menu

/* 재화 관련 코드 */
#pragma region Goods
public:
	UFUNCTION()
	void SyncMetaGoodsUI();

	UFUNCTION()
	void SyncGoldUI();
	
#pragma endregion Goods

/* 보스 체력바 관련 코드 */
#pragma region BossHPBar
public:
	UFUNCTION()
	void OnBossSpawned(AGRLuwoAICharacter* CurrentBoss);

	UFUNCTION()
	void OnBossDestroyed();

#pragma endregion BossHPBar

};
