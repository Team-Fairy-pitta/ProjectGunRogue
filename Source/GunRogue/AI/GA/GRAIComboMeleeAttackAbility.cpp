// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GA/GRAIComboMeleeAttackAbility.h"
#include "Character/GRCharacter.h"
#include "AbilitySystemComponent.h"
#include "AI/Character/GRAICharacter.h"

UGRAIComboMeleeAttackAbility::UGRAIComboMeleeAttackAbility()
	:MaxCombo(4)
	,CapsuleRadius(30.0f)
	,CapsuleHalfHeight(70.0f)
{
	SectionName=FName("MeleeAttack");
	CurCombo=1;
}

void UGRAIComboMeleeAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (ActorInfo->AvatarActor.Get()->HasAuthority())
	{
		PlayAttackMontageAndWaitTask();
		WaitAttackGameplayEventTask();
	}
}

void UGRAIComboMeleeAttackAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	CurCombo=1;
}

void UGRAIComboMeleeAttackAbility::OnAttackTriggerNotify(FGameplayEventData Payload)
{
	Super::OnAttackTriggerNotify(Payload);

	bool bIsAttackSuccess = CanDetectBySweepMulti();
	if (bIsAttackSuccess)
	{
		AttackNextCombo();
	}

	//Impact Cue
	UAbilitySystemComponent* AIASC = GetAbilitySystemComponentFromActorInfo();
	if (AIASC)
	{
		FGameplayTag ImpactCueTag = FGameplayTag::RequestGameplayTag("GameplayCue.AI.Melee.Attack");
		FGameplayCueParameters Params;
		AIASC->ExecuteGameplayCue(ImpactCueTag,Params);
	}
}

bool UGRAIComboMeleeAttackAbility::CanDetectBySweepMulti()
{
	AActor* Instigator = GetAvatarActorFromActorInfo();
	if (!Instigator)
	{
		return false;
	}

	if (!Instigator->HasAuthority())
	{
		return false;
	}

	TArray<FHitResult> HitResults;
	bool bCanMoveNextCombo=false;
	
	FVector Start = Instigator->GetActorLocation();
	FVector End = Start + Instigator->GetActorForwardVector() * CapsuleHalfHeight * 2;
	
	FCollisionShape Shape = FCollisionShape::MakeCapsule(CapsuleRadius, CapsuleHalfHeight);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Instigator);
	FQuat Rotation = FRotationMatrix::MakeFromZ(Instigator->GetActorForwardVector()).ToQuat();
	
	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		Start,
		End,
		Rotation,
		ECC_Pawn,
		Shape,
		QueryParams
	);

#if WITH_EDITOR
	FVector DebugCenter = (Start + End) * 0.5f;
	DrawDebugCapsule(
		GetWorld(),
		DebugCenter,
		CapsuleHalfHeight,
		CapsuleRadius,
		Rotation,
		FColor::Yellow,
		false,
		1.0f
	);
#endif
	
	if (!bHit)
	{
		return false;
	}
	
	for (const FHitResult& Result : HitResults)
	{
		AActor* Other = Result.GetActor();
		if (!Other)
		{
			continue;
		}
		
		AGRCharacter* PlayerChar=Cast<AGRCharacter>(Other);
		if (!PlayerChar)
		{
			continue;
		}

#if WITH_EDITOR
		FVector PlayerLoc = PlayerChar->GetActorLocation();
		if (GetWorld())
		{
			DrawDebugSphere(GetWorld(),PlayerLoc,20.f,12,FColor::Red,false,1.0f);
		}
#endif

		bCanMoveNextCombo=true;
		
		CauseDamage(Other);
	}

	return bCanMoveNextCombo;
}

void UGRAIComboMeleeAttackAbility::AttackNextCombo()
{
	CurCombo++;

	if (CurCombo>MaxCombo)
	{
		return;
	}
	
	AActor* AIActor = GetAvatarActorFromActorInfo();
	if (!AIActor)
	{
		return;
	}

	AGRAICharacter* AICharacter = Cast<AGRAICharacter>(AIActor);
	if (!AICharacter)
	{
		return;
	}

	UAnimInstance* AIAnimInst = AICharacter->GetMesh()->GetAnimInstance();
	if (!AIAnimInst)
	{
		return;
	}
		
	FString SectionNameString = SectionName.ToString() + FString::FromInt(CurCombo);
	AIAnimInst->Montage_JumpToSection(*SectionNameString, AttackMontage);
}
