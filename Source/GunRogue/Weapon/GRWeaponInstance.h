#pragma once

#include "UObject/NoExportTypes.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/GRGameplayEffect.h"
#include "GRWeaponInstance.generated.h"

class UGRWeaponDefinition;
class UGRAbilitySystemComponent;

USTRUCT(BlueprintType)
struct FOptionItem
{
	GENERATED_BODY()

	UPROPERTY()
	FGameplayTag OptionTag;

	UPROPERTY()
	float Value;
};

USTRUCT(BlueprintType)
struct FWeaponOption
{
	GENERATED_BODY()

public:
	UPROPERTY()
	TSubclassOf<UGRGameplayEffect> EffectClass = nullptr;

	UPROPERTY()
	TArray<FOptionItem> OptionItems;

	UPROPERTY()
	bool bIsPercentValue = false;

	UPROPERTY()
	bool bIsAdditivePercent = false;
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

	float GetTotalDamage() const;
	float GetTotalWeakMultuplier() const;
	float GetTotalFireRate() const;
	float GetTotalMagazine() const;

	int32 GetUpgradeCost() const;
	int32 GetRerollCost() const;

	// 탄약 관련 함수
	int32 GetCurrentAmmo() const { return CurrentAmmo; }
	int32 GetMaxAmmo() const;

	bool CheckHasAmmo() const { return CurrentAmmo > 0; }
	bool CheckCanReload() const;

	bool ConsumeAmmo();
	void Reload();

	const TArray<FWeaponOption>& GetOptions() const { return Options; }

public:

	UPROPERTY()
	TArray<FActiveGameplayEffectHandle> AppliedEffects;

	UPROPERTY()
	TObjectPtr<UGRAbilitySystemComponent> CachedASC;

	UPROPERTY()
	TObjectPtr<UGRWeaponDefinition> WeaponDefinition = nullptr;

	// 무기 강화에 필요한 재화 (레벨 비례)
	const int32 UpgradeCostPerLevel = 100;

	// 무기 옵션 리롤에 필요한 재화 (횟수 비례)
	const int32 RerollCostPerCount = 50;

	// 현재 탄약 (이 무기 인스턴스의 탄약 상태)
	UPROPERTY()
	int32 CurrentAmmo;

protected:

	UPROPERTY()
	TArray<FWeaponOption> Options;

	UPROPERTY()
	int32 UpgradeLevel;

	UPROPERTY()
	int32 RerollCount;

	UPROPERTY();
	int8 bIsValid;

	UPROPERTY();
	float CachedTotalDamage;

	UPROPERTY();
	float CachedTotalWeakMultuplier;

	UPROPERTY();
	float CachedTotalFireRate;

	UPROPERTY();
	float CachedTotalMagazine;

	void UpdateCachedAttributes();
};
