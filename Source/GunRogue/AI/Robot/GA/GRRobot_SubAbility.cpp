// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Robot/GA/GRRobot_SubAbility.h"
#include "AbilitySystemComponent.h"
#include "AI/Robot/Drone/GRDroneManagerComponent.h"
#include "AI/Robot/AT/GRSkillAttributeSet_Robot.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"

UGRRobot_SubAbility::UGRRobot_SubAbility()
{
}

void UGRRobot_SubAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
                                          const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	UGRDroneManagerComponent* DroneMgr = Avatar->FindComponentByClass<UGRDroneManagerComponent>();
	if (!DroneMgr)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (!HasAuthority(&ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	DroneMgr->SpawnDrone();

	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}

void UGRRobot_SubAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	if (CooldownGameplayEffectClass)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGameplayEffectClass);
		if (SpecHandle.IsValid() && SpecHandle.Data.Get())
		{
			FGameplayTag CoolDownTag = FGameplayTag::RequestGameplayTag(TEXT("Ability.Cooldown.Magnitude"));
			float FinalCooldown = GetCooldown(ActorInfo);
			SpecHandle.Data.Get()->SetSetByCallerMagnitude(CoolDownTag, FinalCooldown);
			ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
		}
	}
}

float UGRRobot_SubAbility::GetCooldown(const FGameplayAbilityActorInfo* ActorInfo) const
{
	const UGRSkillAttributeSet_Robot* SkillAttributeSet = ActorInfo->AbilitySystemComponent->GetSet<UGRSkillAttributeSet_Robot>();
	const UGRCombatAttributeSet* CombatAttributeSet = ActorInfo->AbilitySystemComponent->GetSet<UGRCombatAttributeSet>();

	if (!SkillAttributeSet || !CombatAttributeSet)
	{
		return 0.0f;
	}
	float BaseCooldown = SkillAttributeSet->GetSubCooldown();
	float CombetSetCooldownReduction = CombatAttributeSet->GetSkillCooldownReduction();
	float FinalCooldown = BaseCooldown * (1.0f - CombetSetCooldownReduction);
	return FinalCooldown;
}

