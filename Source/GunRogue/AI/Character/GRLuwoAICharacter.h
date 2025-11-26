// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/GRAICharacter.h"
#include "GRLuwoAICharacter.generated.h"

/**
 * 
 */
class UGameplayAbility;

UCLASS()
class GUNROGUE_API AGRLuwoAICharacter : public AGRAICharacter
{
	GENERATED_BODY()
	
public:
	AGRLuwoAICharacter();
	
protected:
	virtual void BeginPlay() override;
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities",meta=(AllowPrivateAccess))
	TSubclassOf<UGameplayAbility> GroundStrikeAbilityClass;
};
