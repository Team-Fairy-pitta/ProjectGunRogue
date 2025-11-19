#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemComponent.h"
#include "GRWeaponBase.generated.h"

class UGRGameplayEffect;
class UGRWeaponDataAsset;
class AGRCharacter;
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
	/** 장착 */
	UFUNCTION(BlueprintCallable)
	void EquipWeapon(class AGRCharacter* Character);

	/** 해제 */
	UFUNCTION(BlueprintCallable)
	void UnequipWeapon();

	/** 강화 시도 */
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
	void ApplyDamage();
	void ApplyAllEffects();

	void ClearEffects();

	


public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	UGRWeaponDataAsset* WeaponData;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon|Options")
	TArray<FWeaponOption> Options;

	UPROPERTY()
	UAbilitySystemComponent* CharacterASC;

	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> AppliedEffects;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FWeaponAbility WeaponAbllity;

	UPROPERTY(VisibleAnywhere)
	UBoxComponent* PickupCollision;

};
