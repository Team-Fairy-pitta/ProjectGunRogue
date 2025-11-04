// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TestUIPlayerController.generated.h"

class UGRBattleHUDWidget;
/**
 * 
 */
UCLASS()
class GRTEST_API ATestUIPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UGRBattleHUDWidget> BattleHUDClass;

	UPROPERTY()
	UGRBattleHUDWidget* BattleHUDWidget;
};
