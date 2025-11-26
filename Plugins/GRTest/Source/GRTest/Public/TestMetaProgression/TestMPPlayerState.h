// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "TestMPPlayerState.generated.h"

class UAbilitySystemComponent;
class UGRHealthAttributeSet;
class UGRCombatAttributeSet;
/**
 * 
 */
UCLASS()
class GRTEST_API ATestMPPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	ATestMPPlayerState();

protected:
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GAS")
	TObjectPtr<UAbilitySystemComponent> ASC;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GAS")
	TObjectPtr<UGRHealthAttributeSet> HealthAttributeSet;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="GAS")
	TObjectPtr<UGRCombatAttributeSet> CombatAttributeSet;
};
