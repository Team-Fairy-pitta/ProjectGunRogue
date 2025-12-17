// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Character/GRAICharacter.h"
#include "GRLuwoAICharacter.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterLanded);
DECLARE_MULTICAST_DELEGATE_OneParam(FOnBossHealthChanged, float);


UCLASS()
class GUNROGUE_API AGRLuwoAICharacter : public AGRAICharacter
{
	GENERATED_BODY()
	
public:
	AGRLuwoAICharacter();

	float GetBossHealth() const;

	float GetBossMaxHealth() const;
	
protected:
	virtual void BeginPlay() override;

	virtual void Landed(const FHitResult& Hit) override;

	virtual void EndPlay(EEndPlayReason::Type EndPlayReason) override;

public:
	FOnCharacterLanded OnLandedEvent;

	FOnBossHealthChanged OnBossHealthChanged;
	
};
