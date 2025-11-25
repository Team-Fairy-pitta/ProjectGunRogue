// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "GRAttackAnimNotify_GameplayEvent.generated.h"

/**
 * 
 */

UCLASS()
class GUNROGUE_API UGRAttackAnimNotify_GameplayEvent : public UAnimNotify
{
	GENERATED_BODY()

	virtual void Notify(USkeletalMeshComponent* MeshComp,UAnimSequenceBase* Animation,const FAnimNotifyEventReference& EventReference) override;
	
private:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayAbility",meta=(AllowPrivateAccess))
	FGameplayTag EventTag;
};
