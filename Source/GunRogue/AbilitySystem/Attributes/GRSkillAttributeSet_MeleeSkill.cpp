#include "AbilitySystem/Attributes/GRSkillAttributeSet_MeleeSkill.h"
#include "Net/UnrealNetwork.h"

UGRSkillAttributeSet_MeleeSkill::UGRSkillAttributeSet_MeleeSkill()
{
	// ===== RushSlash =====
	InitRushSlash_BaseDamage(50.0f);
	InitRushSlash_DamageMultiplier(1.0f);
	InitRushSlash_DashDuration(0.35f);
	InitRushSlash_DashForce(1800.0f);
	InitRushSlash_HitRadius(60.0f);
	InitRushSlash_HitCheckInterval(0.02f);
	InitRushSlash_KnockbackStrength(500.0f);
	InitRushSlash_KnockbackUpward(100.0f);          
	InitRushSlash_KnockbackUpgradeMultiplier(1.0f); 

	// ===== BladeWave =====
	InitBladeWave_BaseDamage(150.0f);
	InitBladeWave_DamageMultiplier(1.0f);
	InitBladeWave_BaseFireInterval(1.8f);
	InitBladeWave_FireIntervalMultiplier(1.0f);
	InitBladeWave_BaseWaveScale(1.0f);
	InitBladeWave_BaseWaveScaleMultiplier(1.0f);
}

void UGRSkillAttributeSet_MeleeSkill::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_MeleeSkill, RushSlash_BaseDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_MeleeSkill, RushSlash_DamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_MeleeSkill, RushSlash_DashDuration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_MeleeSkill, RushSlash_DashForce, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_MeleeSkill, RushSlash_HitRadius, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_MeleeSkill, RushSlash_HitCheckInterval, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_MeleeSkill, RushSlash_KnockbackStrength, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_MeleeSkill, RushSlash_KnockbackUpward, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_MeleeSkill, RushSlash_KnockbackUpgradeMultiplier, COND_None, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_MeleeSkill, BladeWave_BaseDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_MeleeSkill, BladeWave_DamageMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_MeleeSkill, BladeWave_BaseFireInterval, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_MeleeSkill, BladeWave_FireIntervalMultiplier, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_MeleeSkill, BladeWave_BaseWaveScale, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_MeleeSkill, BladeWave_BaseWaveScaleMultiplier, COND_None, REPNOTIFY_Always);
}

void UGRSkillAttributeSet_MeleeSkill::OnRep_RushSlash_BaseDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_MeleeSkill, RushSlash_BaseDamage, OldValue);
}
void UGRSkillAttributeSet_MeleeSkill::OnRep_RushSlash_DamageMultiplier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_MeleeSkill, RushSlash_DamageMultiplier, OldValue);
}
void UGRSkillAttributeSet_MeleeSkill::OnRep_RushSlash_DashDuration(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_MeleeSkill, RushSlash_DashDuration, OldValue);
}
void UGRSkillAttributeSet_MeleeSkill::OnRep_RushSlash_DashForce(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_MeleeSkill, RushSlash_DashForce, OldValue);
}
void UGRSkillAttributeSet_MeleeSkill::OnRep_RushSlash_HitRadius(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_MeleeSkill, RushSlash_HitRadius, OldValue);
}
void UGRSkillAttributeSet_MeleeSkill::OnRep_RushSlash_HitCheckInterval(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_MeleeSkill, RushSlash_HitCheckInterval, OldValue);
}
void UGRSkillAttributeSet_MeleeSkill::OnRep_RushSlash_KnockbackStrength(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_MeleeSkill, RushSlash_KnockbackStrength, OldValue);
}
void UGRSkillAttributeSet_MeleeSkill::OnRep_RushSlash_KnockbackUpward(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_MeleeSkill, RushSlash_KnockbackUpward, OldValue);
}
void UGRSkillAttributeSet_MeleeSkill::OnRep_RushSlash_KnockbackUpgradeMultiplier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_MeleeSkill, RushSlash_KnockbackUpgradeMultiplier, OldValue);
}

void UGRSkillAttributeSet_MeleeSkill::OnRep_BladeWave_BaseDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_MeleeSkill, BladeWave_BaseDamage, OldValue);
}
void UGRSkillAttributeSet_MeleeSkill::OnRep_BladeWave_DamageMultiplier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_MeleeSkill, BladeWave_DamageMultiplier, OldValue);
}
void UGRSkillAttributeSet_MeleeSkill::OnRep_BladeWave_BaseFireInterval(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_MeleeSkill, BladeWave_BaseFireInterval, OldValue);
}
void UGRSkillAttributeSet_MeleeSkill::OnRep_BladeWave_FireIntervalMultiplier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_MeleeSkill, BladeWave_FireIntervalMultiplier, OldValue);
}
void UGRSkillAttributeSet_MeleeSkill::OnRep_BladeWave_BaseWaveScale(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_MeleeSkill, BladeWave_BaseWaveScale, OldValue);
}
void UGRSkillAttributeSet_MeleeSkill::OnRep_BladeWave_BaseWaveScaleMultiplier(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_MeleeSkill, BladeWave_BaseWaveScaleMultiplier, OldValue);
}
