// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "GRCheatManager.generated.h"

/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRCheatManager : public UCheatManager
{
	GENERATED_BODY()
	
protected:
	UFUNCTION(exec)
	void Flying();

	UFUNCTION(exec)
	void Walking();

};
