// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/GA/GRAIWeaponFireAbility.h"
#include "GRAIFireWithShotgunAbility.generated.h"

/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRAIFireWithShotgunAbility : public UGRAIWeaponFireAbility
{
	GENERATED_BODY()

public:
	UGRAIFireWithShotgunAbility();

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

private:
	void FireShotgun();
	void LineTraceSingle(const FVector& Direction);
	
private:
	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	int32 PelletCount = 8;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	float SpreadAngle = 6.0f;

	UPROPERTY(EditAnywhere, meta=(AllowPrivateAccess))
	float MaxRange = 1000.0f;
};
