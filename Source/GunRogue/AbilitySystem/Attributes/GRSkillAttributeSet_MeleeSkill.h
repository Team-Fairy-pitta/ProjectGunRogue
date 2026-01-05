#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "GRSkillAttributeSet_MeleeSkill.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)

UCLASS()
class GUNROGUE_API UGRSkillAttributeSet_MeleeSkill : public UAttributeSet
{
	GENERATED_BODY()
	
public:
	UGRSkillAttributeSet_MeleeSkill();

	// Dash
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_RushSlash_BaseDamage, Category = "Skill|RushSlash")
	FGameplayAttributeData RushSlash_BaseDamage;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_MeleeSkill, RushSlash_BaseDamage)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_RushSlash_DamageMultiplier, Category = "Skill|RushSlash")
	FGameplayAttributeData RushSlash_DamageMultiplier;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_MeleeSkill, RushSlash_DamageMultiplier)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_RushSlash_DashDuration, Category = "Skill|RushSlash")
	FGameplayAttributeData RushSlash_DashDuration;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_MeleeSkill, RushSlash_DashDuration)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_RushSlash_DashForce, Category = "Skill|RushSlash")
	FGameplayAttributeData RushSlash_DashForce;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_MeleeSkill, RushSlash_DashForce)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_RushSlash_HitRadius, Category = "Skill|RushSlash")
	FGameplayAttributeData RushSlash_HitRadius;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_MeleeSkill, RushSlash_HitRadius)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_RushSlash_HitCheckInterval, Category = "Skill|RushSlash")
	FGameplayAttributeData RushSlash_HitCheckInterval;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_MeleeSkill, RushSlash_HitCheckInterval)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_RushSlash_KnockbackStrength, Category = "Skill|RushSlash")
	FGameplayAttributeData RushSlash_KnockbackStrength;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_MeleeSkill, RushSlash_KnockbackStrength)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_RushSlash_KnockbackUpward, Category = "Skill|RushSlash")
	FGameplayAttributeData RushSlash_KnockbackUpward;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_MeleeSkill, RushSlash_KnockbackUpward)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_RushSlash_KnockbackUpgradeMultiplier, Category = "Skill|RushSlash")
	FGameplayAttributeData RushSlash_KnockbackUpgradeMultiplier;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_MeleeSkill, RushSlash_KnockbackUpgradeMultiplier)

	
	// BladeWave
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BladeWave_BaseDamage, Category = "Skill|BladeWave")
	FGameplayAttributeData BladeWave_BaseDamage;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_MeleeSkill, BladeWave_BaseDamage)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BladeWave_DamageMultiplier, Category = "Skill|BladeWave")
	FGameplayAttributeData BladeWave_DamageMultiplier;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_MeleeSkill, BladeWave_DamageMultiplier)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BladeWave_BaseFireInterval, Category = "Skill|BladeWave")
	FGameplayAttributeData BladeWave_BaseFireInterval;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_MeleeSkill, BladeWave_BaseFireInterval)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BladeWave_FireIntervalMultiplier, Category = "Skill|BladeWave")
	FGameplayAttributeData BladeWave_FireIntervalMultiplier;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_MeleeSkill, BladeWave_FireIntervalMultiplier)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BladeWave_BaseWaveScale, Category = "Skill|BladeWave")
	FGameplayAttributeData BladeWave_BaseWaveScale;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_MeleeSkill, BladeWave_BaseWaveScale)

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BladeWave_BaseWaveScaleMultiplier, Category = "Skill|BladeWave")
	FGameplayAttributeData BladeWave_BaseWaveScaleMultiplier;
	ATTRIBUTE_ACCESSORS(UGRSkillAttributeSet_MeleeSkill, BladeWave_BaseWaveScaleMultiplier)

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UFUNCTION() void OnRep_RushSlash_BaseDamage(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_RushSlash_DamageMultiplier(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_RushSlash_DashDuration(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_RushSlash_DashForce(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_RushSlash_HitRadius(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_RushSlash_HitCheckInterval(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_RushSlash_KnockbackStrength(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_RushSlash_KnockbackUpward(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_RushSlash_KnockbackUpgradeMultiplier(const FGameplayAttributeData& OldValue);

	UFUNCTION() void OnRep_BladeWave_BaseDamage(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_BladeWave_DamageMultiplier(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_BladeWave_BaseFireInterval(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_BladeWave_FireIntervalMultiplier(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_BladeWave_BaseWaveScale(const FGameplayAttributeData& OldValue);
	UFUNCTION() void OnRep_BladeWave_BaseWaveScaleMultiplier(const FGameplayAttributeData& OldValue);
};
