// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/GA/GRAIAttackAbility.h"
#include "GRAIWeaponFireAbility.generated.h"

/**
 * 
 */

struct FFireContext
{
	AActor* AI;
	AActor* Target;
	AActor* Weapon;
	USkeletalMeshComponent* WeaponMesh;
};

UCLASS()
class GUNROGUE_API UGRAIWeaponFireAbility : public UGRAIAttackAbility
{
	GENERATED_BODY()

public:
	UGRAIWeaponFireAbility();
	
protected:
	bool PrepareFireContext();
	FVector GetAimDirection();
	void SpawnBulletProjectile(const FVector& LaunchDirection);

protected:
	FFireContext FireContext;
};
