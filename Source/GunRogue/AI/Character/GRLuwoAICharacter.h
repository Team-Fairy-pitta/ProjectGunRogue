// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Character/GRAICharacter.h"
#include "GRLuwoAICharacter.generated.h"

/**
 * 
 */
class UGameplayAbility;

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
	UPROPERTY(BlueprintAssignable)
	FOnCharacterLanded OnLandedEvent;
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Abilities",meta=(AllowPrivateAccess))
	TArray<TSubclassOf<UGameplayAbility>> AttackAbilityClassList;
};
