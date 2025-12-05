// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TestAugPlayerController.generated.h"

class UGRBattleHUDWidget;
class UGRAugmentHUDWidget;
class UUserWidget;
/**
 * 
 */
UCLASS()
class GRTEST_API ATestAugPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
	TSubclassOf<UGRBattleHUDWidget> BattleHUDClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = UI)
	TSubclassOf<UGRAugmentHUDWidget> AugmentHUDClass;

	UPROPERTY()
	UGRBattleHUDWidget* BattleHUDWidget;

	UPROPERTY()
	UGRAugmentHUDWidget* AugmentHUDWidget;

protected:
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable)
	void ShowAugmentHUD();

	void ShowBattleHUD();

	void CreateWidgets();
};
