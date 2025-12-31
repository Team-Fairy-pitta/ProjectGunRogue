// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GRDroneProjectile.generated.h"

class UStaticMeshComponent;
class UProjectileMovementComponent;
class USphereComponent;
class UGameplayEffect;

UCLASS()
class GUNROGUE_API AGRDroneProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AGRDroneProjectile();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	
protected:
	UPROPERTY(VisibleAnywhere, Category="Projectile")
	USphereComponent* CollisionComp;
	
	UPROPERTY(VisibleAnywhere, Category="Projectile")
	UStaticMeshComponent* MeshComp;
	
	UPROPERTY(VisibleAnywhere, Category="Projectile")
	UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(EditAnywhere,meta=(AllowPrivateAccess=true))
	float Velocity;

	UPROPERTY(EditAnywhere,meta=(AllowPrivateAccess=true))
	TSubclassOf<UGameplayEffect> DamageGEClass;

	float GetFireDamageFromInstigator() const;
};
