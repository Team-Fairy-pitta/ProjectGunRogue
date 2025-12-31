#include "AbilitySystem/Abilities/MeleeGA/GRAnimNotify_SpawnProjectile.h"

#include "AbilitySystem/Abilities/MeleeGA/GRBladeWaveProjectile.h"
#include "Character/GRCharacter.h"

#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

UGRAnimNotify_SpawnProjectile::UGRAnimNotify_SpawnProjectile()
{
	SocketName = TEXT("Blade");
	bUseAimRotation = true;
}

FString UGRAnimNotify_SpawnProjectile::GetNotifyName_Implementation() const
{
	return TEXT("SpawnBladeWave");
}

static UStaticMeshComponent* FindStaticMeshWithSocket(AGRCharacter* OwnerChar, const FName SocketName)
{
	if (!OwnerChar)
	{
		return nullptr;
	}

	{
		TInlineComponentArray<UStaticMeshComponent*> Comps;
		OwnerChar->GetComponents(Comps);

		for (UStaticMeshComponent* SMC : Comps)
		{
			if (SMC && SMC->DoesSocketExist(SocketName))
			{
				return SMC;
			}
		}
	}

	{
		TArray<AActor*> AttachedActors;
		OwnerChar->GetAttachedActors(AttachedActors);

		for (AActor* A : AttachedActors)
		{
			if (!A) continue;

			TInlineComponentArray<UStaticMeshComponent*> Comps;
			A->GetComponents(Comps);

			for (UStaticMeshComponent* SMC : Comps)
			{
				if (SMC && SMC->DoesSocketExist(SocketName))
				{
					return SMC;
				}
			}
		}
	}

	return nullptr;
}

void UGRAnimNotify_SpawnProjectile::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp)
	{
		return;
	}

	AActor* OwnerActor = MeshComp->GetOwner();
	if (!OwnerActor)
	{
		return;
	}

	AGRCharacter* OwnerChar = Cast<AGRCharacter>(OwnerActor);
	if (!OwnerChar)
	{
		return;
	}

	if (!ProjectileClass)
	{
		return;
	}

	const bool bAuth = OwnerChar->HasAuthority();

	if (!bAuth)
	{
		return;
	}

	UWorld* World = OwnerChar->GetWorld();
	if (!World)
	{
		return;
	}

	UStaticMeshComponent* WeaponMesh = FindStaticMeshWithSocket(OwnerChar, SocketName);
	if (!WeaponMesh)
	{
		return;
	}

	if (!WeaponMesh)
	{
		TArray<AActor*> Attached;
		OwnerChar->GetAttachedActors(Attached);
		for (AActor* A : Attached)
		{
		}
		return;
	}

	if (!WeaponMesh->DoesSocketExist(SocketName))
	{
		return;
	}

	FTransform SpawnTM = WeaponMesh->GetSocketTransform(SocketName, ERelativeTransformSpace::RTS_World);
	if (bUseAimRotation)
	{
		const FRotator AimRot = OwnerChar->GetBaseAimRotation();
		SpawnTM.SetRotation(AimRot.Quaternion());
	}

	AGRBladeWaveProjectile* Proj = World->SpawnActorDeferred<AGRBladeWaveProjectile>(
		ProjectileClass,
		SpawnTM,
		OwnerChar,
		OwnerChar,
		ESpawnActorCollisionHandlingMethod::AlwaysSpawn
	);

	if (!Proj)
	{
		return;
	}

	Proj->FinishSpawning(SpawnTM);

	UE_LOG(LogTemp, Warning, TEXT("[Notify] SUCCESS: Spawned %s at %s"), *GetNameSafe(Proj), *Proj->GetActorLocation().ToString());
}
