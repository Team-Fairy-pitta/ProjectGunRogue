// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Character/GRAICharacter.h"
#include "GRNormalAICharacter.generated.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EGRAIWeaponCategory : uint8
{
	None UMETA(DisplayName = "None"),
	Pistol UMETA(DisplayName = "Pistol"),
	Rifle UMETA(DisplayName = "Rifle"),
	Shotgun UMETA(DisplayName = "Shotgun"),
	SniperRifle UMETA(DisplayName = "SniperRifle"),
};


UCLASS()
class GUNROGUE_API AGRNormalAICharacter : public AGRAICharacter
{
	GENERATED_BODY()

public:
	AGRNormalAICharacter();

	UFUNCTION(BlueprintPure, Category="Weapon")
	EGRAIWeaponCategory GetWeaponType() const {return WeaponCategory;}

	AActor* GetWeaponActor() const {return WeaponActor;}

protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void Tick(float DeltaTime) override;

	virtual void OnDead() override;

private:
	void SpawnAndAttachWeapon();
	void SetWeaponData();
	void Server_UpdateAimOffset(float DeltaTime);
	
	UFUNCTION()
	void OnRep_AimTarget();

protected:
	UPROPERTY(EditAnywhere, Category = "Weapon")
	EGRAIWeaponCategory WeaponCategory;
	
	UPROPERTY()
	TSubclassOf<AActor> WeaponClass;

	UPROPERTY()
	TObjectPtr<AActor> WeaponActor;
	
	FName EquippedSocketName;

	UPROPERTY(EditAnywhere, Category = "Weapon")
	TObjectPtr<UDataTable> WeaponDataTable;

	UPROPERTY(Replicated, BlueprintReadOnly)
	float AimPitch;

	UPROPERTY(Replicated, BlueprintReadOnly)
	float AimYaw;

	UPROPERTY(ReplicatedUsing=OnRep_AimTarget)
	float AimPitchTarget;
	
	UPROPERTY(ReplicatedUsing=OnRep_AimTarget)
	float AimYawTarget;

	float AimUpdateTimer;

	UPROPERTY(EditDefaultsOnly, Category="Aim")
	float AimInterpSpeedDeg;

	UPROPERTY(EditDefaultsOnly, Category="Aim")
	float AimUpdateInterval; 
};
