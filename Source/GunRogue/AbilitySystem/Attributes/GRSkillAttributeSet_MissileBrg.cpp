#include "GRSkillAttributeSet_MissileBrg.h"

#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"

UGRSkillAttributeSet_MissileBrg::UGRSkillAttributeSet_MissileBrg()
{
	// 기본값 설정
	InitBaseDamage(0.0f);
	InitExplosionRadius(0.0f);
	InitExplosionFalloff(0.0f);
	InitProjectileSpeed(0.0f);
	InitGravityScale(0.0f);
	InitLifeSpan(0.0f);
	
	InitMissileCount(0.0f);
	InitSpawnRadius(0.0f);
	InitSpawnHeightOffset(0.0f);

	InitFireInterval(0.0f);
	InitSpawnDelay(0.0f);

	InitBaseCooldown(0.0f);
	InitCooldownReduction(0.0f);  // 0% 감소로 시작

	InitTargetRange(0.0f);
	InitSpreadAngle(0.0f);
}

void UGRSkillAttributeSet_MissileBrg::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_MissileBrg, BaseDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_MissileBrg, ExplosionRadius, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_MissileBrg, ExplosionFalloff, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_MissileBrg, ProjectileSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_MissileBrg, GravityScale, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_MissileBrg, LifeSpan, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_MissileBrg, MissileCount, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_MissileBrg, SpawnRadius, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_MissileBrg, SpawnHeightOffset, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_MissileBrg, FireInterval, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_MissileBrg, SpawnDelay, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_MissileBrg, BaseCooldown, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_MissileBrg, CooldownReduction, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_MissileBrg, TargetRange, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_MissileBrg, SpreadAngle, COND_None, REPNOTIFY_Always);
}

// RepNotify 구현
void UGRSkillAttributeSet_MissileBrg::OnRep_BaseDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_MissileBrg, BaseDamage, OldValue);
}

void UGRSkillAttributeSet_MissileBrg::OnRep_ExplosionRadius(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_MissileBrg, ExplosionRadius, OldValue);
}

void UGRSkillAttributeSet_MissileBrg::OnRep_ExplosionFalloff(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_MissileBrg, ExplosionFalloff, OldValue);
}

void UGRSkillAttributeSet_MissileBrg::OnRep_ProjectileSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_MissileBrg, ProjectileSpeed, OldValue);
}

void UGRSkillAttributeSet_MissileBrg::OnRep_GravityScale(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_MissileBrg, GravityScale, OldValue);
}

void UGRSkillAttributeSet_MissileBrg::OnRep_LifeSpan(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_MissileBrg, LifeSpan, OldValue);
}

void UGRSkillAttributeSet_MissileBrg::OnRep_MissileCount(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_MissileBrg, MissileCount, OldValue);
}

void UGRSkillAttributeSet_MissileBrg::OnRep_SpawnRadius(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_MissileBrg, SpawnRadius, OldValue);
}

void UGRSkillAttributeSet_MissileBrg::OnRep_SpawnHeightOffset(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_MissileBrg, SpawnHeightOffset, OldValue);
}

void UGRSkillAttributeSet_MissileBrg::OnRep_FireInterval(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_MissileBrg, FireInterval, OldValue);
}

void UGRSkillAttributeSet_MissileBrg::OnRep_SpawnDelay(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_MissileBrg, SpawnDelay, OldValue);
}

void UGRSkillAttributeSet_MissileBrg::OnRep_BaseCooldown(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_MissileBrg, BaseCooldown, OldValue);
}

void UGRSkillAttributeSet_MissileBrg::OnRep_CooldownReduction(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_MissileBrg, CooldownReduction, OldValue);
}

void UGRSkillAttributeSet_MissileBrg::OnRep_TargetRange(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_MissileBrg, TargetRange, OldValue);
}

void UGRSkillAttributeSet_MissileBrg::OnRep_SpreadAngle(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_MissileBrg, SpreadAngle, OldValue);
}
