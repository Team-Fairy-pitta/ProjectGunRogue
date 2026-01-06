// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GRRockProjectile.generated.h"

class USphereComponent;
class UGameplayEffect;
class UProjectileMovementComponent;

UCLASS()
class GUNROGUE_API AGRRockProjectile : public AActor
{
	GENERATED_BODY()
	
public:
	AGRRockProjectile();
	
	void Throw(const FVector& LaunchVelocity);
	
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnRep_LaunchVelocity();
protected:
	UPROPERTY(VisibleAnywhere)
	USphereComponent* CollisionComponent;
	
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* MeshComp;

	UPROPERTY(VisibleAnywhere)
	UProjectileMovementComponent* ProjectileMovement;

private:
	UPROPERTY(EditAnywhere,meta=(AllowPrivateAccess=true))
	TSubclassOf<UGameplayEffect> DamageGEClass;

	UPROPERTY(ReplicatedUsing = OnRep_LaunchVelocity)
	FVector ReplicatedLaunchVelocity;
};
