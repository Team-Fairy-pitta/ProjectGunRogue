// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GA/GRAIAttackAbility.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Character/GRCharacter.h"
#include "Engine/OverlapResult.h"

UGRAIAttackAbility::UGRAIAttackAbility()
	:AttackMontage(nullptr)
	,SavedSpecHandle(FGameplayAbilitySpecHandle())
	,SavedActorInfo(nullptr)
	,SavedActivationInfo(FGameplayAbilityActivationInfo())
	,ProjectileClass(nullptr)
	,SphereRadius(500.f)
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGRAIAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	SavedSpecHandle = Handle;
	SavedActorInfo = ActorInfo;
	SavedActivationInfo = ActivationInfo;
	
	if (!ActorInfo || !ActorInfo->AvatarActor.IsValid() || !AttackMontage)
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, true);
		return;
	}
}

void UGRAIAttackAbility::PlayAttackMontageAndWaitTask()
{
	UAbilityTask_PlayMontageAndWait* MontageTask =
		UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this,
			NAME_None,
			AttackMontage,
			1.0f,      // 재생 속도
			NAME_None, // 시작 섹션
			true,      // StopWhenAbilityEnds
			0.0f       // 시작 위치
		);

	MontageTask->OnCompleted.AddDynamic(this, &UGRAIAttackAbility::OnMontageEnded);
	MontageTask->OnInterrupted.AddDynamic(this, &UGRAIAttackAbility::OnMontageEnded);
	MontageTask->OnCancelled.AddDynamic(this, &UGRAIAttackAbility::OnMontageEnded);
	MontageTask->OnBlendOut.AddDynamic(this, &UGRAIAttackAbility::OnMontageEnded);
	MontageTask->ReadyForActivation();
}

void UGRAIAttackAbility::WaitAttackGameplayEventTask()
{
	UAbilityTask_WaitGameplayEvent* WaitEvent = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		HitEventTag,
		SavedActorInfo->AvatarActor.Get(),  // ExternalTarget: 보통 자기 자신
		false,  // OnlyTriggerOnce
		false  // OnlyMatchExact
	);

	WaitEvent->EventReceived.AddDynamic(this, &UGRAIAttackAbility::OnAttackTriggerNotify);
	WaitEvent->ReadyForActivation();
}

void UGRAIAttackAbility::SpawnProjectile()
{
	AActor* AvatarActor = SavedActorInfo->AvatarActor.Get();
	if (!AvatarActor)
	{
		EndAbility(SavedSpecHandle, SavedActorInfo, SavedActivationInfo, true, false);
		return;
	}

	if (!AvatarActor->HasAuthority())
	{
		EndAbility(SavedSpecHandle, SavedActorInfo, SavedActivationInfo, true, false);
		return;
	}
	
	USkeletalMeshComponent* MeshComp = Cast<USkeletalMeshComponent>(AvatarActor->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	if (!MeshComp)
	{
		EndAbility(SavedSpecHandle, SavedActorInfo, SavedActivationInfo, true, false);
		return;
	}
	
	UWorld* World = AvatarActor->GetWorld();
	if (!World)
	{
		EndAbility(SavedSpecHandle, SavedActorInfo, SavedActivationInfo, true, false);
		return;
	}
	
	if (!ProjectileClass)
	{
		EndAbility(SavedSpecHandle, SavedActorInfo, SavedActivationInfo, true, false);
		return;
	}
	
	FVector SpawnLocation =MeshComp->GetSocketLocation(ProjectileSocketName);
	FRotator SpawnRotation =  MeshComp->GetSocketRotation(ProjectileSocketName);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = AvatarActor;
	SpawnParams.Instigator = Cast<APawn>(AvatarActor);
	
	AActor* ProjectileActor = World->SpawnActor<AActor>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
	if (!ProjectileActor)
	{
		EndAbility(SavedSpecHandle, SavedActorInfo, SavedActivationInfo, true, false);
		return;
	}
	
	Projectile = ProjectileActor;
}

void UGRAIAttackAbility::DetectByOverlapSphereMulti()
{
	AActor* Instigator = GetAvatarActorFromActorInfo();
	if (!Instigator)
	{
		return;
	}

	if (!Instigator->HasAuthority())
	{
		return;
	}
	
	FVector Origin = Instigator->GetActorLocation();
	TArray<FOverlapResult> Overlaps;
	FCollisionShape SphereShape = FCollisionShape::MakeSphere(SphereRadius);
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
	DrawDebugSphere(GetWorld(), Origin, SphereRadius, 16, FColor::Yellow, false, 1.0f);
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

#if WITH_EDITOR
		FVector PlayerLoc = PlayerChar->GetActorLocation();
		if (GetWorld())
		{
			DrawDebugSphere(GetWorld(),PlayerLoc,20.f,12,FColor::Red,false,1.0f);
		}
#endif
		
		CauseDamage(Other);
	}
}

void UGRAIAttackAbility::CauseDamage(AActor* Target)
{
	AGRCharacter* PlayerChar=Cast<AGRCharacter>(Target);
	if (!PlayerChar)
	{
		return;
	}
	
	IAbilitySystemInterface* PlayerASI = Cast<IAbilitySystemInterface>(PlayerChar);
	if (!PlayerASI)
	{
		return;
	}
		
	UAbilitySystemComponent* PlayerASC = PlayerASI->GetAbilitySystemComponent();
	if (!PlayerASC)
	{
		return;
	}
		
	UAbilitySystemComponent* AIASC = GetAbilitySystemComponentFromActorInfo();
	if (!AIASC)
	{
		return;
	}

	AActor* Instigator = GetAvatarActorFromActorInfo();
	if (!Instigator)
	{
		return;
	}

	if (!DamageEffectClass)
	{
		return;
	}
	
	FGameplayEffectContextHandle Context = AIASC->MakeEffectContext();
	Context.AddSourceObject(Instigator);

	FGameplayEffectSpecHandle Spec = AIASC->MakeOutgoingSpec(DamageEffectClass, GetAbilityLevel(), Context);
	if (Spec.IsValid())
	{
		AIASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), PlayerASC);
	}
}

void UGRAIAttackAbility::ExecuteGameplayCueAtGround()
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar)
	{
		return;
	}
	
	FVector Start = Avatar->GetActorLocation();
	FVector End   = Start - FVector(0, 0, 500.f);

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Avatar);

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_Visibility,
		Params
	);

	if (!bHit)
	{
		return;
	}
	
	FGameplayCueParameters CueParams;
	CueParams.Location = Hit.ImpactPoint;
	CueParams.Normal   = Hit.ImpactNormal;

	FGameplayTag CueTag = FGameplayTag::RequestGameplayTag(GameplayCueTagName);
	GetAbilitySystemComponentFromActorInfo()->ExecuteGameplayCue(CueTag, CueParams);
}

void UGRAIAttackAbility::OnAttackTriggerNotify(FGameplayEventData Payload)
{
}

void UGRAIAttackAbility::OnMontageEnded()
{
	EndAbility(GetCurrentAbilitySpecHandle(), GetCurrentActorInfo(), GetCurrentActivationInfo(), false, false);
}
