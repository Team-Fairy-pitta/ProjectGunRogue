#pragma once

#include "UObject/NoExportTypes.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/GRGameplayEffect.h"
#include "GRWeaponInstance.generated.h"

class UGRWeaponDefinition;
class UGRAbilitySystemComponent;



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

// 무기의 런타임 정보를 기록하는 구조체
// 강화 정보와 가지고 있는 옵션 정보를 저장한다.
// 무기를 장착하면 PlayerState에 있는 WeaponHandle에 복사된다.
// 무기를 버리면 WeaponActor에 복사된다.
USTRUCT(BlueprintType)
struct GUNROGUE_API FGRWeaponInstance
{
	GENERATED_BODY()

public:
	FGRWeaponInstance();
	FGRWeaponInstance(const FGRWeaponInstance& Other);
	FGRWeaponInstance& operator=(const FGRWeaponInstance& Other);

	void Validate() { bIsValid = 1; }
	void Invalidate() { bIsValid = 0; }
	bool IsValid() const { return bIsValid == 1; }

	void Init(UGRAbilitySystemComponent* ASC, UGRWeaponDefinition* InWeaponDefinition);

	void UpgradeWeapon();

	void ApplyAllEffects();

	void ClearEffects();

	FWeaponOption RandomOption() const;

	void RerollOption(int32 OptionSlotIndex);

	void AllRerollOption();

	int32 GetLevel() const { return UpgradeLevel; };

	float GetDamage() const { return UpgradeDamage; };

public:

	UPROPERTY()
	TArray<FWeaponOption> Options;

	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> AppliedEffects;

	UPROPERTY()
	TObjectPtr<UGRAbilitySystemComponent> CachedASC;

	UPROPERTY()
	TObjectPtr<UGRWeaponDefinition> WeaponDefinition = nullptr;

	

protected:
	UPROPERTY()
	int32 UpgradeLevel;

	UPROPERTY()
	float UpgradeDamage;

	UPROPERTY();
	int8 bIsValid;
};
