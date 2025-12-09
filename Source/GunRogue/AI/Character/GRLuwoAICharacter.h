// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Character/GRAICharacter.h"
#include "GRLuwoAICharacter.generated.h"

/**
 * 
 */

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnCharacterLanded);

UCLASS()
class GUNROGUE_API AGRLuwoAICharacter : public AGRAICharacter
{
	GENERATED_BODY()
	
public:
	AGRLuwoAICharacter();
	
protected:
	virtual void BeginPlay() override;

	virtual void Landed(const FHitResult& Hit) override;

public:
	FOnCharacterLanded OnLandedEvent;
	
};
