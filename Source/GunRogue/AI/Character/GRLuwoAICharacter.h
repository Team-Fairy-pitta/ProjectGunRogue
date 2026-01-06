// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Character/GRAICharacter.h"
#include "GameplayTagContainer.h"
#include "GRLuwoAICharacter.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterLanded);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBossHealthChanged, float);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBossShieldChanged, float);

UCLASS()
class GUNROGUE_API AGRLuwoAICharacter : public AGRAICharacter
{
	GENERATED_BODY()
	
public:
	AGRLuwoAICharacter();
	float GetBossHealth() const;
	float GetBossMaxHealth() const;
	float GetBossShield() const;
	float GetBossMaxShield() const;
	void StartFlyingGC();
	void StopFlyingGC();
	
protected:
	virtual void BeginPlay() override;
	virtual void Landed(const FHitResult& Hit) override;
	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;
	void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode = 0) override;
	
public:
	FOnCharacterLanded OnLandedEvent;
	FOnBossHealthChanged OnBossHealthChanged;
	FOnBossShieldChanged OnBossShieldChanged;

protected:
	FGameplayTag FlyingTag;
};
