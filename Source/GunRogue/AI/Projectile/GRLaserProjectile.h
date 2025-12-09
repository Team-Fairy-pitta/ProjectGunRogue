// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GRLaserProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UGameplayEffect;

UCLASS()
class GUNROGUE_API AGRLaserProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AGRLaserProjectile();
	
	void Launch(const FVector& NormalizeDirection);

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
protected:
	UPROPERTY(VisibleAnywhere)
	USphereComponent* CollisionComponent;
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComp;

private:
	UPROPERTY(EditAnywhere,meta=(AllowPrivateAccess=true))
	float Velocity;

	UPROPERTY(VisibleAnywhere,meta=(AllowPrivateAccess=true))
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;
	
	UPROPERTY(EditAnywhere,meta=(AllowPrivateAccess=true))
	TSubclassOf<UGameplayEffect> DamageGEClass;
};
