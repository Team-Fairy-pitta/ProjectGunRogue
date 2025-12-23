// GRPlayerStatusWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRPlayerStatusWidget.generated.h"

class UGRHPBarWidget;
class UGRBuffIconWidget;
class UWrapBox;
class UImage;
class UTexture2D;
class UGameplayEffect;

/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRPlayerStatusWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UGRHPBarWidget* PlayerHPBar;

	UPROPERTY(meta = (BindWidget))
	UWrapBox* BuffIconContainer;

	UPROPERTY(meta = (BindWidget))
	UImage* PlayerFace;

	UPROPERTY(EditDefaultsOnly, Category="Buff")
	TSubclassOf<UGRBuffIconWidget> BuffIconClass;

	UPROPERTY()
	TArray<UGRBuffIconWidget*> BuffIcons;

	UPROPERTY()
	TArray<TSubclassOf<UGameplayEffect>> BuffEffects;

public:
	void SetPlayerShieldBar(float CurrentShield, float MaxShield);
	void SetPlayerHPBar(float CurrentHP, float MaxHP);

	void SetPlayerHealth(float Value);
	void SetPlayerMaxHealth(float Value);
	void SetPlayerShield(float Value);
	void SetPlayerMaxShield(float Value);

	void SetCharacterThumbnail(UTexture2D* Thumbnail);
	
	UFUNCTION(BlueprintCallable)
	void CreateBuffIcon();

	void AddBuffIcon(TSubclassOf<UGameplayEffect> GameplayEffectClass);
	void RemoveBuffIcon(TSubclassOf<UGameplayEffect> GameplayEffectClass);
};
