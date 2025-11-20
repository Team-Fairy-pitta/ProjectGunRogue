#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemComponent.h"
#include "GRWeaponBase.generated.h"


class UGRGameplayEffect;
class UGRWeaponDataAsset;
class UBoxComponent;


USTRUCT(BlueprintType)
struct FWeaponAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 Level = 0;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float Damage = 0.f;
};

USTRUCT(BlueprintType)
struct FWeaponOption
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TSubclassOf<UGRGameplayEffect> EffectClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float Value = 0.f;
};


UCLASS()
class GUNROGUE_API AGRWeaponBase : public AActor
{
	GENERATED_BODY()
	
public:	
	AGRWeaponBase();

protected:
	virtual void BeginPlay() override;

public:

	UFUNCTION(BlueprintCallable)
	void EquipWeapon(UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable)
	void UnequipWeapon();
	
	UFUNCTION(BlueprintCallable)
	bool TryUpgradeWeapon();

	UFUNCTION()
	void OnOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

private:

	void ApplyAllEffects();

	void ClearEffects();


public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FWeaponAbility WeaponAbllity;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UGRWeaponDataAsset* WeaponData;

	UPROPERTY()
	TArray<FWeaponOption> Options;

	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> AppliedEffects;

	UPROPERTY()
	UAbilitySystemComponent* CachedASC;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* PickupCollision;
	
};
