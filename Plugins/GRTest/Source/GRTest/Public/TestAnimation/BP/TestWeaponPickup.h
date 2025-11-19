#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestWeaponPickup.generated.h"

class USphereComponent;
class USkeletalMeshComponent;
class UTestWeaponAsset;

UCLASS()
class GRTEST_API ATestWeaponPickup : public AActor
{
	GENERATED_BODY()
	
public:	
	ATestWeaponPickup();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USceneComponent* Root;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* WeaponMeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USphereComponent* PickupSphere;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	UTestWeaponAsset* WeaponData;

	virtual void OnConstruction(const FTransform& Transform) override;

protected:
	UFUNCTION()
	void OnPickupSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

public:
	UFUNCTION(BlueprintCallable, Category = "Weapon")
	UTestWeaponAsset* GetWeaponData() const { return WeaponData; }
};
