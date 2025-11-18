// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "TestMPPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class GRTEST_API ATestMPPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void RequestLocalSave();

	UFUNCTION(BlueprintCallable)
	void RequestLocalLoad();

	UFUNCTION(BlueprintCallable)
	void AddMetaGoodsToTest();

	UFUNCTION(BlueprintCallable)
	void PrintPerkInfoRows();
	
private:
	FString GetUniquePlayerId() const;

	UPROPERTY(EditDefaultsOnly)
	UDataTable* PerkTable;
};
