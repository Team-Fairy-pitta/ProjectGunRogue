// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TestUIPlayerController.generated.h"

class UGRBattleHUDWidget;
class UGRTitleHUDWidget;
class UGRInvitationHUDWidget;
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

	UFUNCTION(BlueprintCallable)
	void CreateInvitationSlotInScrollBox();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UGRBattleHUDWidget> BattleHUDClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UGRTitleHUDWidget> TitleHUDClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "UI")
	TSubclassOf<UGRInvitationHUDWidget> InvitationHUDClass;
	
	UPROPERTY(BlueprintReadWrite, Category = "UI")
	UGRBattleHUDWidget* BattleHUDWidget;

	UPROPERTY(BlueprintReadWrite, Category = "UI")
	UGRTitleHUDWidget* TitleHUDWidget;

	UPROPERTY(BlueprintReadWrite, Category = "UI")
	UGRInvitationHUDWidget* InvitationHUDWidget;

private:
	FTimerHandle PlayingTimeHandle;

	FTimerHandle CooldownTimerHandle;

	int32 PlayingTimeSeconds = 0;
	
	void UpdatePlayingTime();
};
