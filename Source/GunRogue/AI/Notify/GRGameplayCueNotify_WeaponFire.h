// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Static.h"
#include "GRGameplayCueNotify_WeaponFire.generated.h"

/**
 * 
 */

class UNiagaraSystem;

UCLASS()
class GUNROGUE_API UGRGameplayCueNotify_WeaponFire : public UGameplayCueNotify_Static
{
	GENERATED_BODY()
	
public:
	UGRGameplayCueNotify_WeaponFire();

protected:
	virtual void  HandleGameplayCue(
		AActor* MyTarget,
		EGameplayCueEvent::Type EventType,
		const FGameplayCueParameters& Parameters
	) override;
	
protected:
	UPROPERTY(EditAnywhere)
	TArray<UParticleSystem*> Particles;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Effect")
	TArray<UNiagaraSystem*> NiagaraSystems;
	
	UPROPERTY(EditAnywhere)
    TArray<USoundBase*> Sounds;

	UPROPERTY(EditAnywhere)
	UAnimMontage* WeaponFireMontage;
};
