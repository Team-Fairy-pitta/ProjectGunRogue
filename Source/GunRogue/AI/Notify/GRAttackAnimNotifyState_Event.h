// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "GRAttackAnimNotifyState_Event.generated.h"

/**
 * 
 */

UCLASS()
class GUNROGUE_API UGRAttackAnimNotifyState_Event : public UAnimNotifyState
{
	GENERATED_BODY()
	
protected:
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp,
						 UAnimSequenceBase* Animation,
						 float TotalDuration,
						 const FAnimNotifyEventReference& EventReference) override;

private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayAbility",meta=(AllowPrivateAccess))
	FGameplayTag EventTag;
};
