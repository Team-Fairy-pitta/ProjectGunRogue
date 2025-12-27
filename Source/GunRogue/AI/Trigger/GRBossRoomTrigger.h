// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TriggerVolume.h"
#include "GRBossRoomTrigger.generated.h"

/**
 * 
 */
UCLASS()
class GUNROGUE_API AGRBossRoomTrigger : public ATriggerVolume
{
	GENERATED_BODY()
	
public:
	AGRBossRoomTrigger();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnOverlapBegin(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnOverlapEnd(AActor* OverlappedActor, AActor* OtherActor);

protected:
	UPROPERTY(EditAnywhere)
	APawn* BossPawn;
};
