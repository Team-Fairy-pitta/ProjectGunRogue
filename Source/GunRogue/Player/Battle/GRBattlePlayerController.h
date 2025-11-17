#pragma once

#include "Player/GRPlayerController.h"
#include "GRBattlePlayerController.generated.h"

class UGRBattleHUDWidget;
struct FGameplayEffectSpec;

UCLASS()
class GUNROGUE_API AGRBattlePlayerController : public AGRPlayerController
{
	GENERATED_BODY()
	
public:
	AGRBattlePlayerController();
	virtual void BeginPlay() override;
	virtual void OnRep_PlayerState() override;

protected:
	UPROPERTY()
	TObjectPtr<UGRBattleHUDWidget> HUDWidgetInstance;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget|Class")
	TSubclassOf<UGRBattleHUDWidget> HUDWidgetClass;

	void ShowBattleHUD();

private:
	void CreateWidgets();
	void InitBattleHUD();
	void InitUISetup();

	void UpdatePlayerHealth(float Value);
	void UpdatePlayerMaxHealth(float Value);
	void UpdatePlayerShield(float Value);
	void UpdatePlayerMaxShield(float Value);

	void OnHealthChanged(AActor* EventInstigator, AActor* Causer, const FGameplayEffectSpec* Spec, float Magnitude, float OldValue, float NewValue);
	void OnMaxHealthChanged(AActor* EventInstigator, AActor* Causer, const FGameplayEffectSpec* Spec, float Magnitude, float OldValue, float NewValue);
	void OnShieldChanged(AActor* EventInstigator, AActor* Causer, const FGameplayEffectSpec* Spec, float Magnitude, float OldValue, float NewValue);
	void OnMaxShieldChanged(AActor* EventInstigator, AActor* Causer, const FGameplayEffectSpec* Spec, float Magnitude, float OldValue, float NewValue);
};
