#pragma once

#include "Player/GRPlayerController.h"
#include "GRBattlePlayerController.generated.h"

class UGRBattleHUDWidget;
class UGRWeaponUpgrade;
struct FGameplayEffectSpec;
struct FOnAttributeChangeData;

UCLASS()
class GUNROGUE_API AGRBattlePlayerController : public AGRPlayerController
{
	GENERATED_BODY()
	
public:
	AGRBattlePlayerController();
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	virtual void OnRep_PlayerState() override;

	// Duration이 있는 (무제한 포함) Effect가 추가 되었을 때 호출됨 (Instance는 호출되지 않음)
	UFUNCTION(Client, Reliable)
	void ClientRPC_OnActiveGameplayEffectAdded(TSubclassOf<UGameplayEffect> EffectClass);

	// Duration이 있는 (무제한 포함) Effect가 제거 되었을 때 호출됨 (Instance는 호출되지 않음)
	UFUNCTION(Client, Reliable)
	void ClientRPC_OnActiveGameplayEffectRemoved(TSubclassOf<UGameplayEffect> EffectClass);

protected:
	UPROPERTY()
	TObjectPtr<UGRBattleHUDWidget> HUDWidgetInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget|Class")
	TSubclassOf<UGRBattleHUDWidget> HUDWidgetClass;

	FTimerHandle OtherPlayerStatusUpdateTimer;
	float OtherPlayerStatusUpdateInterval = 1.0f;

	void ShowBattleHUD();

private:
	void CreateWidgets();
	void InitBattleHUD();
	void InitUISetup();

	void UpdatePlayerHealth(float Value);
	void UpdatePlayerMaxHealth(float Value);
	void UpdatePlayerShield(float Value);
	void UpdatePlayerMaxShield(float Value);

	void OnUpdateOtherPlayerStatus();

	void OnHealthChanged(const FOnAttributeChangeData& Data);
	void OnMaxHealthChanged(const FOnAttributeChangeData& Data);
	void OnShieldChanged(const FOnAttributeChangeData& Data);
	void OnMaxShieldChanged(const FOnAttributeChangeData& Data);

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
	TSubclassOf<UUserWidget> UpgradeConsoleWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> UpgradeConsoleWidgetInstance;

#pragma endregion UpgradeConsole
};
