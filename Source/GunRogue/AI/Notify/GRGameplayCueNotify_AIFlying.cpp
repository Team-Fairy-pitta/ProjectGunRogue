// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Notify/GRGameplayCueNotify_AIFlying.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"

AGRGameplayCueNotify_AIFlying::AGRGameplayCueNotify_AIFlying()
	:AttachSocketName(FName("JetBack"))
{
	bAutoDestroyOnRemove = true;
}

bool AGRGameplayCueNotify_AIFlying::OnActive_Implementation(AActor* Target, const FGameplayCueParameters& Parameters)
{
	if (!Target)
	{
		return false;
	}

	USkeletalMeshComponent* Mesh = Target->FindComponentByClass<USkeletalMeshComponent>();
	if (!Mesh)
	{
		return false;
	}
	
	if (JetFX)
	{
		JetFXComponent = UNiagaraFunctionLibrary::SpawnSystemAttached(
			JetFX,
			Mesh,
			AttachSocketName,
			FVector::ZeroVector,
			FRotator::ZeroRotator,
			EAttachLocation::SnapToTarget,
			false);
	}

	if (JetSound)
	{
		JetAudioComponent = UGameplayStatics::SpawnSoundAttached(
			JetSound,
			Mesh,
			AttachSocketName);
	}

	return true;
}

bool AGRGameplayCueNotify_AIFlying::OnRemove_Implementation(AActor* Target, const FGameplayCueParameters& Parameters)
{
	if (JetFXComponent)
	{
		JetFXComponent->Deactivate();
	}

	if (JetAudioComponent)
	{
		JetAudioComponent->Stop();
	}

	return true;
}

