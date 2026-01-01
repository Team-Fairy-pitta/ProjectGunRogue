// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GRDroneAIController.generated.h"

/**
 * 
 */

class UStateTree;
class UStateTreeComponent;

UCLASS()
class GUNROGUE_API AGRDroneAIController : public AAIController
{
	GENERATED_BODY()
	
public:
	AGRDroneAIController();
	
protected:
	virtual void OnPossess(APawn* InPawn) override;
	
};
