#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "GRCombatAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnAmmoChanged, int32, CurrentAmmo, int32, MaxAmmo);

UCLASS()
class GUNROGUE_API UGRCombatAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UGRCombatAttributeSet();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	// 탄퍼짐 증가 (사격 시 호출)
	void IncreaseSpread(UAbilitySystemComponent* OwningASC);

	// 탄약, 장전 가능 여부 확인
	bool CheckHasAmmo() const { return GetCurrentAmmo() > 0.0f; }
	bool CheckCanReload() const { return GetCurrentAmmo() < GetMaxAmmo(); }

	// 탄약 델리게이트 (블루프린트에서 바인딩 가능)
	UPROPERTY(BlueprintAssignable, Category = "GunRogue|Ammo")
	FOnAmmoChanged OnAmmoChanged;

	// AttributeSet 업데이트 (UI 표시용)
	void UpdateAmmoDisplay(int32 InCurrentAmmo, int32 InMaxAmmo);

	// 무기 기본 공격력
	UPROPERTY(BlueprintReadOnly, Category = "Combat|WeaponDamage", ReplicatedUsing = OnRep_WeaponDamage_Base)
	FGameplayAttributeData WeaponDamage_Base;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, WeaponDamage_Base)

	// 무기 공격력 증가 (Flat Addition)
	UPROPERTY(BlueprintReadOnly, Category = "Combat|WeaponDamage", ReplicatedUsing = OnRep_WeaponDamage_Additive)
	FGameplayAttributeData WeaponDamage_Additive;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, WeaponDamage_Additive)

	// 무기 공격력 증폭 (Multiplier)
	UPROPERTY(BlueprintReadOnly, Category = "Combat|WeaponDamage", ReplicatedUsing = OnRep_WeaponDamage_Multiplicative)
	FGameplayAttributeData WeaponDamage_Multiplicative;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, WeaponDamage_Multiplicative)


	// 기본 약점 배율
	UPROPERTY(BlueprintReadOnly, Category = "Combat|Critical", ReplicatedUsing = OnRep_WeaponCriticalMultiplier_Base)
	FGameplayAttributeData WeaponCriticalMultiplier_Base;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, WeaponCriticalMultiplier_Base)

	// 약점 배율 증폭
	UPROPERTY(BlueprintReadOnly, Category = "Combat|Critical", ReplicatedUsing = OnRep_WeaponCriticalMultiplier_Multiplicative)
	FGameplayAttributeData WeaponCriticalMultiplier_Multiplicative;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, WeaponCriticalMultiplier_Multiplicative)

	// 약점 배율 추가 증가
	UPROPERTY(BlueprintReadOnly, Category = "Combat|Critical", ReplicatedUsing = OnRep_WeaponCriticalMultiplier_Bonus)
	FGameplayAttributeData WeaponCriticalMultiplier_Bonus;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, WeaponCriticalMultiplier_Bonus)

	// 일반 배율 (약점이 아닐 때)
	UPROPERTY(BlueprintReadOnly, Category = "Combat|Critical", ReplicatedUsing = OnRep_WeaponNormalMultiplier_Multiplicative)
	FGameplayAttributeData WeaponNormalMultiplier_Multiplicative;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, WeaponNormalMultiplier_Multiplicative)


	// 최종 피해 배율 증가
	UPROPERTY(BlueprintReadOnly, Category = "Combat|FinalDamage", ReplicatedUsing = OnRep_FinalDamage_Additive)
	FGameplayAttributeData FinalDamage_Additive;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, FinalDamage_Additive)

	// 최종 피해 배율 증폭
	UPROPERTY(BlueprintReadOnly, Category = "Combat|FinalDamage", ReplicatedUsing = OnRep_FinalDamage_Multiplicative)
	FGameplayAttributeData FinalDamage_Multiplicative;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, FinalDamage_Multiplicative)

	// 최종 피해 배율 추가 증가
	UPROPERTY(BlueprintReadOnly, Category = "Combat|FinalDamage", ReplicatedUsing = OnRep_FinalDamage_Bonus)
	FGameplayAttributeData FinalDamage_Bonus;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, FinalDamage_Bonus)


	// 피해 감소 배율 (적용 시)
	UPROPERTY(BlueprintReadOnly, Category = "Combat|Defense", ReplicatedUsing = OnRep_DamageReduction)
	FGameplayAttributeData DamageReduction;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, DamageReduction)


	// 약점 공격 여부 (Meta)
	UPROPERTY(BlueprintReadOnly, Category = "Combat|Meta")
	FGameplayAttributeData IsCriticalHit;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, IsCriticalHit)

	// 연사력 (초당 발사 횟수, RoundsPerMinute / 60)
    UPROPERTY(BlueprintReadOnly, Category = "Combat|Weapon", ReplicatedUsing = OnRep_FireRate)
    FGameplayAttributeData FireRate;
    ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, FireRate)

    // 집탄율 (0.0 ~ 1.0, 높을수록 정확함)
    UPROPERTY(BlueprintReadOnly, Category = "Combat|Weapon", ReplicatedUsing = OnRep_Accuracy)
    FGameplayAttributeData Accuracy;
    ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, Accuracy)

    // 반동 (크기, 0.0 ~ 무한대)
    UPROPERTY(BlueprintReadOnly, Category = "Combat|Weapon", ReplicatedUsing = OnRep_Recoil)
    FGameplayAttributeData Recoil;
    ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, Recoil)

    // 탄퍼짐 회복 속도 (초당 회복률)
    UPROPERTY(BlueprintReadOnly, Category = "Combat|Weapon", ReplicatedUsing = OnRep_SpreadRecoveryRate)
    FGameplayAttributeData SpreadRecoveryRate;
    ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, SpreadRecoveryRate)

    // 최대 탄퍼짐 (각도, degree)
    UPROPERTY(BlueprintReadOnly, Category = "Combat|Weapon", ReplicatedUsing = OnRep_MaxSpread)
    FGameplayAttributeData MaxSpread;
    ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, MaxSpread)

	// 탄퍼짐 증가량 (발사 시마다 추가되는 각도)
	UPROPERTY(BlueprintReadOnly, Category = "Combat|Weapon", ReplicatedUsing = OnRep_SpreadIncreasePerShot)
	FGameplayAttributeData SpreadIncreasePerShot;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, SpreadIncreasePerShot)

	// 현재 탄퍼짐 (각도)
	UPROPERTY(BlueprintReadOnly, Category = "Combat|Weapon", ReplicatedUsing = OnRep_CurrentSpread)
	FGameplayAttributeData CurrentSpread;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, CurrentSpread)

	// 탄퍼짐 회복 관련 고정 상수
	static constexpr float SPREAD_RECOVERY_DELAY = 0.2f;      // 사격 멈추고 회복시작 하기까지 딜레이(인터벌보다 커야 사격 중 회복 안 됨)
	static constexpr float SPREAD_RECOVERY_INTERVAL = 0.1f;   // 0.1초마다 회복 틱

	// 현재 탄약
	UPROPERTY(BlueprintReadOnly, Category = "Combat|Ammo", ReplicatedUsing = OnRep_CurrentAmmo)
	FGameplayAttributeData CurrentAmmo;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, CurrentAmmo)

	// 최대 탄약 (탄창 크기)
	UPROPERTY(BlueprintReadOnly, Category = "Combat|Ammo", ReplicatedUsing = OnRep_MaxAmmo)
	FGameplayAttributeData MaxAmmo;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, MaxAmmo)

	// 재장전 속도 (1.0 = 기본, 높을수록 빠름)
	UPROPERTY(BlueprintReadOnly, Category = "Combat|Weapon", ReplicatedUsing = OnRep_ReloadRate)
	FGameplayAttributeData ReloadRate;
	ATTRIBUTE_ACCESSORS(UGRCombatAttributeSet, ReloadRate)


	// 무기 데미지 계산 (무기 공격력만)
	float CalculateWeaponDamage() const;

	// 약점 배율 계산
	float CalculateCriticalMultiplier(bool bIsCritical) const;

	// 최종 피해 배율 계산
	float CalculateFinalDamageMultiplier() const;

	// 최종 데미지 계산 (모든 요소 통합)
	float CalculateFinalWeaponDamage(bool bIsCritical, float TargetDamageReduction) const;

protected:
	UFUNCTION()
	virtual void OnRep_WeaponDamage_Base(const FGameplayAttributeData& OldWeaponDamageBase);

	UFUNCTION()
	virtual void OnRep_WeaponDamage_Additive(const FGameplayAttributeData& OldWeaponDamageAdditive);

	UFUNCTION()
	virtual void OnRep_WeaponDamage_Multiplicative(const FGameplayAttributeData& OldWeaponDamageMultiplicative);

	UFUNCTION()
	virtual void OnRep_WeaponCriticalMultiplier_Base(const FGameplayAttributeData& OldWeaponCriticalMultiplierBase);

	UFUNCTION()
	virtual void OnRep_WeaponCriticalMultiplier_Multiplicative(const FGameplayAttributeData& OldWeaponCriticalMultiplierMultiplicative);

	UFUNCTION()
	virtual void OnRep_WeaponCriticalMultiplier_Bonus(const FGameplayAttributeData& OldWeaponCriticalMultiplierBonus);

	UFUNCTION()
	virtual void OnRep_WeaponNormalMultiplier_Multiplicative(const FGameplayAttributeData& OldWeaponNormalMultiplierMultiplicative);

	UFUNCTION()
	virtual void OnRep_FinalDamage_Additive(const FGameplayAttributeData& OldFinalDamageAdditive);

	UFUNCTION()
	virtual void OnRep_FinalDamage_Multiplicative(const FGameplayAttributeData& OldFinalDamageMultiplicative);

	UFUNCTION()
	virtual void OnRep_FinalDamage_Bonus(const FGameplayAttributeData& OldFinalDamageBonus);

	UFUNCTION()
	virtual void OnRep_DamageReduction(const FGameplayAttributeData& OldDamageReduction);

	UFUNCTION()
	virtual void OnRep_FireRate(const FGameplayAttributeData& OldFireRate);

	UFUNCTION()
	virtual void OnRep_Accuracy(const FGameplayAttributeData& OldAccuracy);

	UFUNCTION()
	virtual void OnRep_Recoil(const FGameplayAttributeData& OldRecoil);

	UFUNCTION()
	virtual void OnRep_SpreadRecoveryRate(const FGameplayAttributeData& OldSpreadRecoveryRate);

	UFUNCTION()
	virtual void OnRep_MaxSpread(const FGameplayAttributeData& OldMaxSpread);

	UFUNCTION()
	virtual void OnRep_SpreadIncreasePerShot(const FGameplayAttributeData& OldSpreadIncreasePerShot);

	UFUNCTION()
	virtual void OnRep_CurrentSpread(const FGameplayAttributeData& OldCurrentSpread);

	UFUNCTION()
	virtual void OnRep_CurrentAmmo(const FGameplayAttributeData& OldCurrentAmmo);

	UFUNCTION()
	virtual void OnRep_MaxAmmo(const FGameplayAttributeData& OldMaxAmmo);
	
	UFUNCTION()
	virtual void OnRep_ReloadRate(const FGameplayAttributeData& OldReloadRate);

private:
	// 탄퍼짐 자동 회복을 위한 타이머
	TMap<TWeakObjectPtr<UAbilitySystemComponent>, FTimerHandle> SpreadRecoveryTimers;
	void StartSpreadRecoveryTimer(UAbilitySystemComponent* OwningASC, bool bIsInitialDelay = true);
	void ClearSpreadRecoveryTimer(UAbilitySystemComponent* OwningASC);
	void ApplySpreadRecovery(UAbilitySystemComponent* OwningASC);

};
