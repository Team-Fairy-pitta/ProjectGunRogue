// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TestMPPlayerController.generated.h"

class UGRPerkHUDWidget;
/**
 * 
 */
UCLASS()
class GRTEST_API ATestMPPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;
	
	UFUNCTION(BlueprintCallable)
	void SetMetaGoodsInText();

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perk")
	TSubclassOf<UGRPerkHUDWidget> PerkHUDClass;

	UPROPERTY(BlueprintReadOnly, Category="Perk")
	UGRPerkHUDWidget* PerkHUDWidget;

private:
	
	UPROPERTY(EditDefaultsOnly)
	UDataTable* PerkTable;
};
