// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TestUIPlayerController.generated.h"

class UGRBattleHUDWidget;
class UInputMappingContext;
class UInputAction;
/**
 * 
 */
UCLASS()
class GRTEST_API ATestUIPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	UInputMappingContext* InputMappingContext = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	UInputAction* MoveAction = nullptr;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UGRBattleHUDWidget> BattleHUDClass;

	UPROPERTY()
	UGRBattleHUDWidget* BattleHUDWidget;
};
