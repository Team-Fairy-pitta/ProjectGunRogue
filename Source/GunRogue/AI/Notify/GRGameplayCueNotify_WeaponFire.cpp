// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Notify/GRGameplayCueNotify_WeaponFire.h"
#include "AI/Character/GRNormalAICharacter.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SkeletalMeshComponent.h"
#include "NiagaraFunctionLibrary.h"

UGRGameplayCueNotify_WeaponFire::UGRGameplayCueNotify_WeaponFire()
{
}

void  UGRGameplayCueNotify_WeaponFire::HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType,
                                                        const FGameplayCueParameters& Parameters)
{
	if (EventType == EGameplayCueEvent::Executed)
	{
		if (Particle)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				MyTarget->GetWorld(),
				Particle,
				Parameters.Location,
				Parameters.Normal.Rotation()
			);
		}

		if (NiagaraSystems.Num() > 0)
		{
			for (UNiagaraSystem* System : NiagaraSystems)
			{
				if (System)
				{
					UNiagaraFunctionLibrary::SpawnSystemAtLocation(
						MyTarget->GetWorld(),
						System,
						Parameters.Location,
						Parameters.Normal.Rotation()
					);
				}
			}
		}
		
		if (Sound)
		{
			UGameplayStatics::PlaySoundAtLocation(
				MyTarget->GetWorld(),
				Sound,
				Parameters.Location
			);
		}

		if (WeaponFireMontage)
		{
			if (MyTarget)
			{
				AGRNormalAICharacter* NormalAIChar = Cast<AGRNormalAICharacter>(MyTarget);
				if (NormalAIChar)
				{
					AActor* Weapon = NormalAIChar->GetWeaponActor();
					if (Weapon)
					{
						USkeletalMeshComponent* MeshComp = Weapon->FindComponentByClass<USkeletalMeshComponent>();
	            
						if (MeshComp)
						{
							UAnimInstance* AnimInst = MeshComp->GetAnimInstance();
							if (AnimInst)
							{
								AnimInst->Montage_Play(WeaponFireMontage);
							}
						}	
					}
					else
					{
						UE_LOG(LogTemp,Warning,TEXT("AI Debug"));
					}
				}
				else
				{
					UE_LOG(LogTemp,Warning,TEXT("AI Debug"));
				}
			}
		}
	}
}
