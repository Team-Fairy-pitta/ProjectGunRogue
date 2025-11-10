// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TestUIPlayerController.generated.h"

class UGRBattleHUDWidget;
class UGRTitleHUDWidget;
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

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	UInputAction* SkillAction_Q = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	UInputAction* SkillAction_E = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Input")
	UInputAction* SkillAction_Shift = nullptr;

	UFUNCTION()
	void HandleSkillInput(FName SkillKey);

	UFUNCTION(BlueprintCallable)
	void SelectWeaponSlot(int32 Index);

	UFUNCTION(BlueprintCallable)
	void CreateBuffIconInWrapBox();

	UFUNCTION(BlueprintCallable)
	void CreateTeamStatusInVerticalBox();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UGRBattleHUDWidget> BattleHUDClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UGRTitleHUDWidget> TitleHUDClass;
	
	UPROPERTY()
	UGRBattleHUDWidget* BattleHUDWidget;

	UPROPERTY()
	UGRTitleHUDWidget* TitleHUDWidget;

private:
	FTimerHandle PlayingTimeHandle;

	FTimerHandle CooldownTimerHandle;

	int32 PlayingTimeSeconds = 0;
	
	void UpdatePlayingTime();
};
