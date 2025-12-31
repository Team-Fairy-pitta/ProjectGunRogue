// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GRRockProjectile.generated.h"

class USphereComponent;
class UGameplayEffect;

UCLASS()
class GUNROGUE_API AGRRockProjectile : public AActor
{
	GENERATED_BODY()
	
public:
	AGRRockProjectile();
	
	void Throw(const FVector& LaunchVelocity);
	
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
	TSubclassOf<UGameplayEffect> DamageGEClass;

	UPROPERTY(EditAnywhere)
	float TorqueStrength;
};
