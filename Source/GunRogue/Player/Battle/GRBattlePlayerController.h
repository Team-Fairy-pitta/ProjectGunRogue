#pragma once

#include "Player/GRPlayerController.h"
#include "GRBattlePlayerController.generated.h"

class UGRBattleHUDWidget;
struct FGameplayEffectSpec;
struct FOnAttributeChangeData;

UCLASS()
class GUNROGUE_API AGRBattlePlayerController : public AGRPlayerController
{
	GENERATED_BODY()
	
public:
	AGRBattlePlayerController();
	virtual void BeginPlay() override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayResaon) override;
	virtual void OnRep_PlayerState() override;

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
};
