// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GA/GRJumpAndAttackAbility.h"
#include "AI/Character/GRLuwoAICharacter.h"
#include "AI/Controller/GRBossLuwoAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "NavigationSystem.h"
#include "AbilitySystemComponent.h"
#include "Character/GRCharacter.h"
#include "Engine/OverlapResult.h"

UGRJumpAndAttackAbility::UGRJumpAndAttackAbility()
{
}

void UGRJumpAndAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	JumpToTargetLocation();
}

void UGRJumpAndAttackAbility::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);

	AGRLuwoAICharacter* Boss = Cast<AGRLuwoAICharacter>(SavedActorInfo->AvatarActor.Get());
	if (Boss)
	{
		if (Boss->OnLandedEvent.IsAlreadyBound(this, &UGRJumpAndAttackAbility::OnLanded))
		{
			Boss->OnLandedEvent.RemoveDynamic(this, &UGRJumpAndAttackAbility::OnLanded);
		}
	}
}

void UGRJumpAndAttackAbility::OnHitNotify(FGameplayEventData Payload)
{
	Super::OnHitNotify(Payload);
	
	AActor* Instigator = GetAvatarActorFromActorInfo();
	if (!Instigator) return;

	FVector Origin = Instigator->GetActorLocation();
	const float Radius = 500.f;
	TArray<FOverlapResult> Overlaps;

	FCollisionShape SphereShape = FCollisionShape::MakeSphere(Radius);
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(Instigator);
	
	ECollisionChannel TraceChannel = ECC_Pawn;

	bool bOverlap = GetWorld()->OverlapMultiByChannel(
		Overlaps,
		Origin,               
		FQuat::Identity,      
		TraceChannel,
		SphereShape,
		QueryParams
	);

	
#if WITH_EDITOR
	DrawDebugSphere(GetWorld(), Origin, Radius, 16, FColor::Yellow, false, 1.0f);
#endif
	
	if (!bOverlap)
	{
		return;
	}
	
	for (const FOverlapResult& Result : Overlaps)
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

		IAbilitySystemInterface* PlayerASI = Cast<IAbilitySystemInterface>(PlayerChar);
		if (!PlayerASI)
		{
			continue;
		}
		
		UAbilitySystemComponent* PlayerASC = PlayerASI->GetAbilitySystemComponent();
		if (!PlayerASC)
		{
			continue;
		}
		
		UAbilitySystemComponent* AIASC = GetAbilitySystemComponentFromActorInfo();
		if (!AIASC)
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
		
		FGameplayEffectContextHandle Context = AIASC->MakeEffectContext();
		Context.AddSourceObject(Instigator);

		FGameplayEffectSpecHandle Spec = AIASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), Context);
		if (Spec.IsValid())
		{
			AIASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), PlayerASC);
		}
	}
}

void UGRJumpAndAttackAbility::JumpToTargetLocation()
{
	AGRLuwoAICharacter* Boss = Cast<AGRLuwoAICharacter>(SavedActorInfo->AvatarActor.Get());
	if (!Boss)
	{
		EndAbility(SavedSpecHandle, SavedActorInfo, SavedActivationInfo, true, true);
		return;
	}
	
	AGRBossLuwoAIController* BossCon=Cast<AGRBossLuwoAIController>(Boss->GetController());
	if (!BossCon)
	{
		EndAbility(SavedSpecHandle, SavedActorInfo, SavedActivationInfo, true, true);
		return;
	}
	
	UBlackboardComponent* BB=BossCon->GetBlackboardComponent();
	if (!BB)
	{
		EndAbility(SavedSpecHandle, SavedActorInfo, SavedActivationInfo, true, true);
		return;
	}

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(AGRBossLuwoAIController::TargetPlayerKey));
	if (!TargetActor)
	{
		EndAbility(SavedSpecHandle, SavedActorInfo, SavedActivationInfo, true, true);
		return;
	}

	FVector Start = Boss->GetActorLocation();
	FVector TargetLocation = TargetActor->GetActorLocation();
	
	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(GetWorld());
	FNavLocation NavLoc;
	if (NavSys && NavSys->ProjectPointToNavigation(TargetLocation, NavLoc))
	{
		TargetLocation = NavLoc.Location;
	}
	else
	{
		EndAbility(SavedSpecHandle, SavedActorInfo, SavedActivationInfo, true, true);
		return;
	}
	
	FVector LaunchVel;
	const float ArcParam=0.5f;
	bool bHaveSolution = UGameplayStatics::SuggestProjectileVelocity_CustomArc(
		Boss,
		LaunchVel,
		Start,
		TargetLocation,
		/*OverrideGravityZ*/ Boss -> GetWorld()->GetGravityZ(),
		/*곡선의 높이 조절 -낮으면 더 위로, 높으면 더 낮게*/ ArcParam);
	
	if (!bHaveSolution)
	{
		EndAbility(SavedSpecHandle, SavedActorInfo, SavedActivationInfo, true, true);
		return;
	}
	
	Boss->LaunchCharacter(LaunchVel, true, true);
	
	if (!Boss->OnLandedEvent.IsAlreadyBound(this, &UGRJumpAndAttackAbility::OnLanded))
	{
		Boss->OnLandedEvent.AddDynamic(this, &UGRJumpAndAttackAbility::OnLanded);
	}
}

void UGRJumpAndAttackAbility::OnLanded()
{
	PlayAttackMontageAndWaitTask();
	WaitAttackGameplayEventTask();
}
