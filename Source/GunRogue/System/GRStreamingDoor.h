// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GRStreamingDoor.generated.h"

class UArrowComponent;

UCLASS()
class GUNROGUE_API AGRStreamingDoor : public AActor
{
	GENERATED_BODY()
	
public:	

	AGRStreamingDoor();

	UFUNCTION(BlueprintImplementableEvent)
	void ActivateDoor();
	
protected:

	virtual void BeginPlay() override;

};
