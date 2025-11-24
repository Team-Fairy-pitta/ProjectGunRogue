#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemComponent.h"
#include "GRWeaponBase.generated.h"


class UGRGameplayEffect;
class UGRWeaponDataAsset;
class UBoxComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnWeaponStatChanged);

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

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;


public:

	UFUNCTION(BlueprintCallable)
	void EquipWeapon(UAbilitySystemComponent* ASC);

	UFUNCTION(Server, Reliable)
	void Server_EquipWeapon(UAbilitySystemComponent* ASC);

	UFUNCTION(BlueprintCallable)
	void UnequipWeapon();

	UFUNCTION(Server, Reliable)
	void Server_UnequipWeapon();
	
	UFUNCTION(BlueprintCallable)
	void TryUpgradeWeapon();

	UFUNCTION(Server, Reliable)
	void Server_TryUpgradeWeapon(); 

	UFUNCTION()
	void OnOverlap(
		UPrimitiveComponent* OverlappedComp,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	int32 GetLevel() const { return WeaponAbility.Level; }

	float GetDamage() const { return WeaponAbility.Damage; }

	UFUNCTION()
	void OnRep_WeaponAbility();

private:
	void ApplyAllEffects();

	void ClearEffects();


public:

	UPROPERTY(ReplicatedUsing = OnRep_WeaponAbility)
	FWeaponAbility WeaponAbility;

	UPROPERTY(BlueprintAssignable)
	FOnWeaponStatChanged OnWeaponStatChanged;

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
