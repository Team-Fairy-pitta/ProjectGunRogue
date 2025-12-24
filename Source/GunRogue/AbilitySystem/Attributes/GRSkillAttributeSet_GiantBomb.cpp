#include "AbilitySystem/Attributes/GRSkillAttributeSet_GiantBomb.h"
#include "Net/UnrealNetwork.h"

UGRSkillAttributeSet_GiantBomb::UGRSkillAttributeSet_GiantBomb()
{
	// 기본값 설정
	InitBaseDamage(0.0f);
	InitExplosionRadius(0.0f);
	InitExplosionFalloff(0.5f);
	InitFuseTime(0.0f);
	InitKnockbackForce(0.0f);
	InitBaseCooldown(0.0f);
	InitCooldownReduction(0.0f);
}

void UGRSkillAttributeSet_GiantBomb::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_GiantBomb, BaseDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_GiantBomb, ExplosionRadius, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_GiantBomb, ExplosionFalloff, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_GiantBomb, FuseTime, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_GiantBomb, KnockbackForce, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_GiantBomb, BaseCooldown, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_GiantBomb, CooldownReduction, COND_None, REPNOTIFY_Always);
}

void UGRSkillAttributeSet_GiantBomb::OnRep_BaseDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_GiantBomb, BaseDamage, OldValue);
}

void UGRSkillAttributeSet_GiantBomb::OnRep_ExplosionRadius(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_GiantBomb, ExplosionRadius, OldValue);
}

void UGRSkillAttributeSet_GiantBomb::OnRep_ExplosionFalloff(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_GiantBomb, ExplosionFalloff, OldValue);
}

void UGRSkillAttributeSet_GiantBomb::OnRep_FuseTime(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_GiantBomb, FuseTime, OldValue);
}

void UGRSkillAttributeSet_GiantBomb::OnRep_KnockbackForce(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_GiantBomb, KnockbackForce, OldValue);
}

void UGRSkillAttributeSet_GiantBomb::OnRep_BaseCooldown(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_GiantBomb, BaseCooldown, OldValue);
}

void UGRSkillAttributeSet_GiantBomb::OnRep_CooldownReduction(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_GiantBomb, CooldownReduction, OldValue);
}
