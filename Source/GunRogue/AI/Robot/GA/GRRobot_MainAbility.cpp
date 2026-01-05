// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Robot/GA/GRRobot_MainAbility.h"
#include "AI/Robot/Drone/GRDroneManagerComponent.h"
#include "AI/Robot/AT/GRSkillAttributeSet_Robot.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"


UGRRobot_MainAbility::UGRRobot_MainAbility()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::ServerInitiated;
}

bool UGRRobot_MainAbility::CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, OUT FGameplayTagContainer* OptionalRelevantTags) const
{
	if (Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags))
	{
		AActor* Avatar = ActorInfo->AvatarActor.Get();
		if (!Avatar)
		{
			return false;
		}

		UGRDroneManagerComponent* DroneMgr = Avatar->FindComponentByClass<UGRDroneManagerComponent>();
		if (!DroneMgr)
		{
			return false;
		}

		if (DroneMgr->GetDrones().Num() == 0)
		{
			return false;
		}

		return true;
	}
	else
	{
		return false;
	}
}

void UGRRobot_MainAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
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
	
	if (!HasAuthority(&ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
	
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
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
	
	if (DroneMgr->GetDrones().Num() == 0)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}

	DroneMgr->CommandAllDronesAttack(GetAimDirection());

	EndAbility(Handle, ActorInfo, ActivationInfo, false, false);
}

void UGRRobot_MainAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
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

FVector UGRRobot_MainAbility::GetAimDirection()
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar)
	{
		return FVector::ZeroVector;
	}

	APlayerController* PC = Cast<APlayerController>(Cast<APawn>(Avatar)->GetController());

	if (!PC)
	{
		return FVector::ZeroVector;
	}

	FVector CamLoc;
	FRotator CamRot;
	PC->GetPlayerViewPoint(CamLoc, CamRot);

	return CamRot.Vector();
}

float UGRRobot_MainAbility::GetCooldown(const FGameplayAbilityActorInfo* ActorInfo) const
{
	const UGRSkillAttributeSet_Robot* SkillAttributeSet = ActorInfo->AbilitySystemComponent->GetSet<UGRSkillAttributeSet_Robot>();
	const UGRCombatAttributeSet* CombatAttributeSet = ActorInfo->AbilitySystemComponent->GetSet<UGRCombatAttributeSet>();

	if (!SkillAttributeSet || !CombatAttributeSet)
	{
		return 0.0f;
	}
	float BaseCooldown = SkillAttributeSet->GetMainCooldown();
	float CombetSetCooldownReduction = CombatAttributeSet->GetSkillCooldownReduction();
	float FinalCooldown = BaseCooldown * (1.0f - CombetSetCooldownReduction);
	return FinalCooldown;
}

