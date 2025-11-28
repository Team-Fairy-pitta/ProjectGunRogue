// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GA/GRBoulderTossAttackAbility.h"
#include "AI/Projectile/GRRockProjectile.h"
#include "AI/Character/GRLuwoAICharacter.h"
#include "AI/Controller/GRBossLuwoAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"

UGRBoulderTossAttackAbility::UGRBoulderTossAttackAbility()
	:Projectile(nullptr)
	,ProjectileClass(nullptr)
	,ProjectileSocketName(TEXT("ProjectilePosition"))
{
}

void UGRBoulderTossAttackAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	SpawnProjectile();
	PlayAttackMontageAndWaitTask();
	WaitAttackGameplayEventTask();
}

void UGRBoulderTossAttackAbility::OnAttackTriggerNotify(FGameplayEventData Payload)
{
	Super::OnAttackTriggerNotify(Payload);

	AGRRockProjectile* RockProjectile=Cast<AGRRockProjectile>(Projectile);
	if (!RockProjectile)
	{
		EndAbility(SavedSpecHandle, SavedActorInfo, SavedActivationInfo, true, false);
		return;
	}

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

	FVector StartLocation = RockProjectile->GetActorLocation();
	FVector TargetLocation = TargetActor->GetActorLocation();

	FVector LaunchVelocity;
	const float ArcParam=0.5f;
	bool bHaveSolution = UGameplayStatics::SuggestProjectileVelocity_CustomArc(
		 GetWorld(),
		 LaunchVelocity,
		 StartLocation,
		 TargetLocation,
		 GetWorld()->GetGravityZ(),
		 ArcParam 
	);

	if (bHaveSolution)
	{
		// optional: 디버그로 launch 벡터 표시
		DrawDebugDirectionalArrow(GetWorld(), StartLocation, StartLocation + LaunchVelocity * 0.1f, 60, FColor::Cyan, false, 2.0f, 0, 5.0f);
		
		RockProjectile->Throw(LaunchVelocity);
	}
	else
	{
		EndAbility(SavedSpecHandle, SavedActorInfo, SavedActivationInfo, true, true);
		return;
	}
	
	Projectile=nullptr;
	
}

void UGRBoulderTossAttackAbility::SpawnProjectile()
{
	AActor* AvatarActor = SavedActorInfo->AvatarActor.Get();
	if (!AvatarActor)
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
	
	AGRRockProjectile* RockProjectile = World->SpawnActor<AGRRockProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);
	if (!RockProjectile)
	{
		EndAbility(SavedSpecHandle, SavedActorInfo, SavedActivationInfo, true, false);
		return;
	}
	
	Projectile = RockProjectile;
}
