// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "GameplayCueNotifyTypes.h"
#include "GRGameplayCueNotify_AIFlying.generated.h"

/**
 * 
 */
 
 class UNiagaraSystem;
 class UNiagaraComponent;
class USoundBase;
class UAudioComponent;
 
UCLASS()
class GUNROGUE_API AGRGameplayCueNotify_AIFlying : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()
public:
	AGRGameplayCueNotify_AIFlying();
	
	virtual bool OnActive_Implementation(
		AActor* Target,
		const FGameplayCueParameters& Parameters) override;

	virtual bool OnRemove_Implementation(
		AActor* Target,
		const FGameplayCueParameters& Parameters) override;
protected:
	UPROPERTY(EditDefaultsOnly, Category = "FX")
	UNiagaraSystem* JetFX;

	UPROPERTY(EditDefaultsOnly, Category = "Sound")
	USoundBase* JetSound;

	UPROPERTY(EditDefaultsOnly, Category = "FX")
	TArray<FName> AttachSocketNames;
	
	UPROPERTY()
	TArray<TObjectPtr<UNiagaraComponent>> JetFXComponents;

	UPROPERTY()
	TArray<TObjectPtr<UAudioComponent>> JetAudioComponents;
};
