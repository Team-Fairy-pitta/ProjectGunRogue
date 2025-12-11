// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Player/Battle/GRBattlePlayerController.h"
#include "TestUIDamageIndicator.generated.h"

class UGRDamageIndicator;
/**
 * 
 */
UCLASS()
class GRTEST_API ATestUIDamageIndicator : public AGRBattlePlayerController
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	void ShowDamageIndicator(float Damage, AActor* DamagedActor);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget|Class")
	TSubclassOf<UGRDamageIndicator> DamageIndicatorWidgetClass;

	UPROPERTY()
	TObjectPtr<UGRDamageIndicator> DamageIndicatorWidgetInstance;

	
};
