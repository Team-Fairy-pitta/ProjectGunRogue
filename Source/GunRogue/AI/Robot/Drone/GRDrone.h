// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Pawn.h"
#include "GRDrone.generated.h"

class UFloatingPawnMovement;
class UGameplayEffect;

UENUM(BlueprintType)
enum class EDroneState : uint8
{
	Idle,
	Fire,
	MainSkillAttack
};

UCLASS()
class GUNROGUE_API AGRDrone : public APawn
{
	GENERATED_BODY()

public:
	AGRDrone();
	void SetTargetOffset(AActor* Player,int32 Index,int TotalNum);
	void StartMainSkillAttack(const FVector& InDir);
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UFUNCTION()
	void OnDetectBegin(UPrimitiveComponent* Overlapped,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 BodyIndex,
		bool bFromSweep,
		const FHitResult& Hit);

	UFUNCTION()
	void OnDetectEnd(UPrimitiveComponent* Overlapped,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 BodyIndex);

private:
	void FollowPlayer();
	void FaceForward();
	void SetCloseTarget();
	bool CanFireAtTarget(AActor* Target);
	void UpdateState();
	void FaceTarget();
	void Fire();
	void DashToDirection();
	void Explode();
	void ApplyMovement();
	void UpdateDroneStateMachine();
	
private:
	UPROPERTY(VisibleAnywhere, Category="Components")
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, Category="Components")
	UStaticMeshComponent* Mesh;
	
	UPROPERTY()
	AActor* FollowTarget;

	UPROPERTY(EditAnywhere,Category="Follow")
	float CircleRadius;

	UPROPERTY(EditAnywhere,Category="Follow")
	FVector CircleCenter;

	FVector TargetOffset;

	UPROPERTY(EditAnywhere, Category="Follow")
	float MoveInterpSpeed;

	UPROPERTY(EditAnywhere, Category="Follow")
	float RotationInterpSpeed;
	
	UPROPERTY(EditAnywhere, Category="Follow")
	float StopDistance;

	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> DetectedEnemies;
	
	UPROPERTY(VisibleAnywhere, Category="Components")
	USphereComponent* DetectSphere;

	UPROPERTY()
	AActor* AITarget;

	UPROPERTY()
	EDroneState DroneState = EDroneState::Idle;

	float FireCooldown = 0.f;

	UPROPERTY(EditAnywhere, Category="Fire")
	float FireInterval = 2.0f;

	float TargetUpdateTimer = 0.f;
	float TargetUpdateInterval = 0.2f;

	UPROPERTY(EditDefaultsOnly, Category = "Fire")
	TSubclassOf<AActor> BulletClass;

	FName MuzzleSocketName = TEXT("Muzzle");
	FVector MainSkillDir;

	UPROPERTY(EditDefaultsOnly, Category = "MainSkillAttack")
	float MainSkillSpeed = 1000.f;

	bool bIsMainSkillActive = false;

	UPROPERTY(EditAnywhere, Category="MainSkillAttack")
	TSubclassOf<UGameplayEffect> ExplodeDamageGEClass;
	
	FTimerHandle ExplodeTimerHandle;

	UPROPERTY(Replicated)
	FVector DesiredLocation;

	UPROPERTY(Replicated)
	FRotator DesiredRotation;

	float GetDurationFromInstigator() const;
	float GetFireIntervalFromInstigator() const;
	float GetExplodeDistanceFromInstigator() const;
	float GetExplodeDamageFromInstigator() const;
};
