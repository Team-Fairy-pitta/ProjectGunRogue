// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Robot/AT/GRSkillAttributeSet_Robot.h"
#include "Net/UnrealNetwork.h"

UGRSkillAttributeSet_Robot::UGRSkillAttributeSet_Robot()
{
	InitDamage(10.0f);
	InitDuration(10.0f);
	InitAttackSpeed(1.0f);
	InitSelfDestructDamage(100.0f);
	InitSelfDestructRadius(300.0f);
	InitMainCooldown(15.0f);
	InitSubCooldown(3.0f);
}

void UGRSkillAttributeSet_Robot::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_Robot, Damage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_Robot, Duration, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_Robot, AttackSpeed, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_Robot, SelfDestructDamage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_Robot, SelfDestructRadius, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_Robot, MainCooldown, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UGRSkillAttributeSet_Robot, SubCooldown, COND_None, REPNOTIFY_Always);
}

void UGRSkillAttributeSet_Robot::OnRep_Damage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_Robot, Damage, OldValue);
}

void UGRSkillAttributeSet_Robot::OnRep_Duration(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_Robot, Duration, OldValue);
}

void UGRSkillAttributeSet_Robot::OnRep_AttackSpeed(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_Robot, AttackSpeed, OldValue);
}

void UGRSkillAttributeSet_Robot::OnRep_SelfDestructDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_Robot, SelfDestructDamage, OldValue);
}

void UGRSkillAttributeSet_Robot::OnRep_SelfDestructRadius(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_Robot, SelfDestructRadius, OldValue);
}

void UGRSkillAttributeSet_Robot::OnRep_MainCooldown(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_Robot, MainCooldown, OldValue);
}

void UGRSkillAttributeSet_Robot::OnRep_SubCooldown(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UGRSkillAttributeSet_Robot, SubCooldown, OldValue);
}
