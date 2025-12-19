// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "GRAIWeaponData.generated.h"
/**
 * 
 */
class UGameplayAbility;

USTRUCT(BlueprintType)
struct FGRAIWeaponData : public FTableRowBase
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AActor> WeaponClass = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName EquipSocketName = "RightHandSocket";

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UGameplayAbility> AbilityToUse = nullptr;
};
