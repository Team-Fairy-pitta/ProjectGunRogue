// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GA/GRAIWeaponFireAbility.h"
#include "AI/Character/GRNormalAICharacter.h"
#include "AI/Controller/GRAIController.h"
#include "BehaviorTree/BlackboardComponent.h"

UGRAIWeaponFireAbility::UGRAIWeaponFireAbility()
{
	ProjectileSocketName = FName("muzzle");
}

bool UGRAIWeaponFireAbility::PrepareFireContext()
{
	AActor* AvatarActor = SavedActorInfo->AvatarActor.Get();
	if (!AvatarActor)
	{
		return false;
	}

	if (!AvatarActor->HasAuthority())
	{
		return false;
	}
	
	UWorld* World = AvatarActor->GetWorld();
	if (!World)
	{
		return false;
	}

	AGRNormalAICharacter* NoramlAIChar=Cast<AGRNormalAICharacter>(AvatarActor);
	if (!NoramlAIChar)
	{
		return false;
	}

	AGRAIController* AICon=Cast<AGRAIController>(NoramlAIChar->GetController());
	if (!AICon)
	{
		return false;
	}
	
	UBlackboardComponent* BB=AICon->GetBlackboardComponent();
	if (!BB)
	{
		return false;
	}

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(AGRAIController::TargetPlayerKey));
	if (!TargetActor)
	{
		return false;
	}

	if (!NoramlAIChar->HasAuthority())
	{
		return false;
	}

	AActor* AIWeapon = NoramlAIChar->GetWeaponActor();
	if (!AIWeapon)
	{
		return false;
	}
	
	USkeletalMeshComponent* WeaponMeshComp = Cast<USkeletalMeshComponent>(AIWeapon->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	if (!WeaponMeshComp)
	{
		return false;
	}

	FireContext.AI = AvatarActor;
	FireContext.Target = TargetActor;
	FireContext.Weapon = AIWeapon;
	FireContext.WeaponMesh = WeaponMeshComp;
	
	return true;
}

FVector UGRAIWeaponFireAbility::GetAimDirection()
{
	FVector StartLocation =FireContext.WeaponMesh->GetSocketLocation(ProjectileSocketName);
	FVector TargetLocation = FireContext.Target->GetActorLocation();
	FVector LaunchDirection=(TargetLocation - StartLocation).GetSafeNormal();

	return LaunchDirection;
}

void UGRAIWeaponFireAbility::SpawnBulletProjectile(const FVector& LaunchDirection)
{
	FRotator SpawnRotation = LaunchDirection.Rotation();
	
	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = FireContext.Weapon;
	SpawnParams.Instigator = Cast<APawn>(FireContext.AI);
	
	AActor* ProjectileActor = GetWorld()->SpawnActor<AActor>(ProjectileClass, FireContext.WeaponMesh->GetSocketLocation(ProjectileSocketName), SpawnRotation, SpawnParams);
	if (!ProjectileActor)
	{
		EndAbility(SavedSpecHandle, SavedActorInfo, SavedActivationInfo, true, false);
		return;
	}
	
	if (UPrimitiveComponent* ColComp = Cast<UPrimitiveComponent>(ProjectileActor->GetComponentByClass(UPrimitiveComponent::StaticClass())))
	{
		ColComp->IgnoreActorWhenMoving(FireContext.Weapon, true);
		ColComp->IgnoreActorWhenMoving(FireContext.AI, true);
	}
}
