// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TestMPPlayerController.generated.h"

class UGRPerkHUDWidget;
class UGameplayEffect;
class UGRPerkSlotWidget;
/**
 * 
 */
UCLASS()
class GRTEST_API ATestMPPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void OnPossess(APawn* InPawn) override;
	
	UFUNCTION(BlueprintCallable)
	void SetMetaGoodsInText();

	UFUNCTION()
	void ApplyPerkToASCOnSlotClicked(UGRPerkSlotWidget* PerkSlotWidget);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perk")
	TSubclassOf<UGRPerkHUDWidget> PerkHUDClass;

	UPROPERTY(BlueprintReadOnly, Category="Perk")
	UGRPerkHUDWidget* PerkHUDWidget;

private:
	UPROPERTY(EditDefaultsOnly, Category="Perk")
	UDataTable* PerkTable;

	UPROPERTY(EditAnywhere, Category="Perk")
	TSubclassOf<UGameplayEffect> PerkGE;
};
