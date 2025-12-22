#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "GRSkillAttributeSet_MissileBrg.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
    GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

/**
 * Missile Barrage 스킬 전용 AttributeSet
 * 캐릭터 주변에 원형으로 미사일 생성 후 바라보는 타겟 방향으로 발사.
 */

UCLASS()
class GUNROGUE_API UGRSkillAttributeSet_MissileBrg : public UAttributeSet
{
	GENERATED_BODY()

public:
	UGRSkillAttributeSet_MissileBrg();

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	// ======== 투사체 능력치 ========

	// 기본 데미지
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDamage, Category = "Skill|MissileBarrage")
	FGameplayAttributeData BaseDamage;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_MissileBrg, BaseDamage)

	// 폭발 반경
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ExplosionRadius, Category = "Skill|MissileBarrage")
	FGameplayAttributeData ExplosionRadius;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_MissileBrg, ExplosionRadius)

	// 폭발 감쇠 (0.0 = 가장자리도 풀뎀, 1.0 = 가장자리 0뎀)
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ExplosionFalloff, Category = "Skill|MissileBarrage")
	FGameplayAttributeData ExplosionFalloff;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_MissileBrg, ExplosionFalloff)

	// 투사체 속도
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_ProjectileSpeed, Category = "Skill|MissileBarrage")
	FGameplayAttributeData ProjectileSpeed;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_MissileBrg, ProjectileSpeed)

	// 중력 스케일
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_GravityScale, Category = "Skill|MissileBarrage")
	FGameplayAttributeData GravityScale;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_MissileBrg, GravityScale)

	// 투사체 수명
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_LifeSpan, Category = "Skill|MissileBarrage")
	FGameplayAttributeData LifeSpan;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_MissileBrg, LifeSpan)

	// ======== 스폰 패턴 ========

	// 미사일 개수
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MissileCount, Category = "Skill|MissileBarrage")
	FGameplayAttributeData MissileCount;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_MissileBrg, MissileCount)

	// 스폰 반경
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_SpawnRadius, Category = "Skill|MissileBarrage")
	FGameplayAttributeData SpawnRadius;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_MissileBrg, SpawnRadius)

	// 스폰 높이 오프셋
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_SpawnHeightOffset, Category = "Skill|MissileBarrage")
	FGameplayAttributeData SpawnHeightOffset;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_MissileBrg, SpawnHeightOffset)

	// ======== 타이밍 ========

	// 발사 간격
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_FireInterval, Category = "Skill|MissileBarrage")
	FGameplayAttributeData FireInterval;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_MissileBrg, FireInterval)

	// 스폰 딜레이
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_SpawnDelay, Category = "Skill|MissileBarrage")
	FGameplayAttributeData SpawnDelay;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_MissileBrg, SpawnDelay)

	// ======== 쿨타임 ========

	// 기본 쿨타임
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseCooldown, Category = "Skill|MissileBarrage")
	FGameplayAttributeData BaseCooldown;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_MissileBrg, BaseCooldown)

	// 쿨타임 감소율 (0.0 ~ 1.0, 곱연산용)
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_CooldownReduction, Category = "Skill|MissileBarrage")
	FGameplayAttributeData CooldownReduction;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_MissileBrg, CooldownReduction)

	// ======== 조준 ========

	// 타겟 범위
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_TargetRange, Category = "Skill|MissileBarrage")
	FGameplayAttributeData TargetRange;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_MissileBrg, TargetRange)

	// 퍼짐 각도
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_SpreadAngle, Category = "Skill|MissileBarrage")
	FGameplayAttributeData SpreadAngle;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_MissileBrg, SpreadAngle)

protected:
	// RepNotify 함수들
	UFUNCTION()
	virtual void OnRep_BaseDamage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_ExplosionRadius(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_ExplosionFalloff(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_ProjectileSpeed(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_GravityScale(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_LifeSpan(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_MissileCount(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_SpawnRadius(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_SpawnHeightOffset(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_FireInterval(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_SpawnDelay(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_BaseCooldown(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_CooldownReduction(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_TargetRange(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	virtual void OnRep_SpreadAngle(const FGameplayAttributeData& OldValue);
	
};
