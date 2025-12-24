#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRBossBarWidget.generated.h"

class UTextBlock;
class UProgressBar;
class AGRLuwoAICharacter;

UCLASS()
class GUNROGUE_API UGRBossBarWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION()
	void SetBoss(AGRLuwoAICharacter* InBoss);

	void SetHealthBar(float CurrentHP, float MaxHP);

	void SetHealth(float Value);
	void SetMaxHealth(float Value);

	void SetShieldBar(float CurrentSD, float MaxSD);

	void SetShield(float Value);
	void SetMaxShield(float Value);

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	
public:
	UPROPERTY(meta = (BindWidget))
	UTextBlock* BossNameText;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BossHealthBarText;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* BossHealthBar;	

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BossShieldBarText;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* BossShieldBar;

private:
	UPROPERTY()
	AGRLuwoAICharacter* BossCharacter;

private:
	float Health = 0;
	float MaxHealth = 0;

	float Shield = 0;
	float MaxShield = 0;

	FText BossName;
};
