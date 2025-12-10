// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRDamageIndicator.generated.h"

/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRDamageIndicator : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void SetData(float Damage, AActor* TargetActor);
	
};
