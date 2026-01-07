// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Notify/GRGameplayCueNotify_AIFlying.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraComponent.h"
#include "Components/AudioComponent.h"

AGRGameplayCueNotify_AIFlying::AGRGameplayCueNotify_AIFlying()
{
	bAutoDestroyOnRemove = true;
	AttachSocketNames.Add(FName("JetBack_L"));
	AttachSocketNames.Add(FName("JetBack_R"));
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
		FRotator JetRotation = FRotator(180.f, 0.f, 0.f);

		for (FName AttachSocketName : AttachSocketNames)
		{
			UNiagaraComponent* FXComp =
				UNiagaraFunctionLibrary::SpawnSystemAttached(
					JetFX,
					Mesh,
					AttachSocketName,
					FVector::ZeroVector,
					JetRotation,
					EAttachLocation::SnapToTarget,
					false);
			if (FXComp)
			{
				JetFXComponents.Add(FXComp);
			}
		}
	}

	if (JetSound)
	{
		for (const FName& AttachSocketName : AttachSocketNames)
		{
			UAudioComponent* AudioComp =UGameplayStatics::SpawnSoundAttached(JetSound,Mesh,AttachSocketName);
			if (AudioComp)
			{
				JetAudioComponents.Add(AudioComp);
			}
		}
	}

	return true;
}

bool AGRGameplayCueNotify_AIFlying::OnRemove_Implementation(AActor* Target, const FGameplayCueParameters& Parameters)
{
	for (UNiagaraComponent* FXComp : JetFXComponents)
	{
		if (FXComp)
		{
			FXComp->Deactivate();
		}
	}
	JetFXComponents.Empty();

	for (UAudioComponent* AudioComp : JetAudioComponents)
	{
		if (AudioComp)
		{
			AudioComp->Stop();
		}
	}
	JetAudioComponents.Empty();

	return true;
}

