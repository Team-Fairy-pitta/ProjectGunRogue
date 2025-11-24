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

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

private:
	// 보낼 GameplayEvent의 태그
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GameplayAbility",meta=(AllowPrivateAccess))
	FGameplayTag EventTag;
};
