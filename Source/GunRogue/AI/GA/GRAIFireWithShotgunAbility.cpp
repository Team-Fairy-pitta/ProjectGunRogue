// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GA/GRAIFireWithShotgunAbility.h"

UGRAIFireWithShotgunAbility::UGRAIFireWithShotgunAbility()
{
}

void UGRAIFireWithShotgunAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	if (ActorInfo->AvatarActor.Get()->HasAuthority())
	{
		PlayAttackMontageAndWaitTask();

		if (!PrepareFireContext())
		{
			EndAbility(SavedSpecHandle, SavedActorInfo, SavedActivationInfo, true, false);
		}
		
		FireShotgun();
	}
}

void UGRAIFireWithShotgunAbility::FireShotgun()
{
	FVector BaseDir =GetAimDirection();

	for (int32 i =0; i < PelletCount; ++i)
	{
		FVector Dir = FMath::VRandCone(
			BaseDir,
			FMath::DegreesToRadians(SpreadAngle)
		);
		
		LineTraceSingle(Dir);
	}
}

void UGRAIFireWithShotgunAbility::LineTraceSingle(const FVector& Direction)
{
	FVector Start = FireContext.WeaponMesh->GetSocketLocation(ProjectileSocketName);
	FVector End = Start + Direction * MaxRange;

	FCollisionQueryParams Params;
	Params.AddIgnoredActor( FireContext.AI);
	Params.AddIgnoredActor( FireContext.Weapon);

	FCollisionObjectQueryParams ObjectParams;
	ObjectParams.AddObjectTypesToQuery(ECC_Pawn);
	ObjectParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjectParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	
	FHitResult Hit;

	bool bHit = GetWorld()->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_GameTraceChannel3, // Weapon Channel
		Params
	);
	
#if WITH_EDITOR
	FVector DebugEnd = bHit ? Hit.ImpactPoint : End;
	
	DrawDebugLine(
	GetWorld(),
	Start,
	DebugEnd,
	FColor::Yellow,
	false,   // 지속 여부
	1.0f,    // 유지 시간
	0,
	1.5f     // 두께
);
#endif
	
	if (bHit)
	{
		if (Hit.GetActor() == FireContext.Target)
		{
			CauseDamage(FireContext.Target);

#if WITH_EDITOR
			DrawDebugSphere(
				GetWorld(),
				Hit.ImpactPoint,
				8.f,
				12,
				FColor::Red,
				false,
				1.0f
			);
#endif
		}
	}
}
