// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameUserSettings.h"
#include "GRGameUserSettings.generated.h"

/**
 * 
 */
UCLASS(config=Game, Blueprintable)
class GUNROGUE_API UGRGameUserSettings : public UGameUserSettings
{
	GENERATED_BODY()

public:
	UPROPERTY(Config)
	float MasterVolume = 1.0f;

	UPROPERTY(Config)
	float BgmVolume = 1.0f;

	UPROPERTY(Config)
	float SfxVolume = 1.0f;

	UFUNCTION(BlueprintCallable, Category = "Volume Setting")
	void SetMasterVolume(float InVolume);

	UFUNCTION(BlueprintCallable, Category = "Volume Setting")
	void SetBgmVolume(float InVolume);

	UFUNCTION(BlueprintCallable, Category = "Volume Setting")
	void SetSfxVolume(float InVolume);

	UFUNCTION(BlueprintPure, Category = "Volume Setting")
	float GetMasterVolume() const;

	UFUNCTION(BlueprintPure, Category = "Volume Setting")
	float GetBgmVolume() const;

	UFUNCTION(BlueprintPure, Category = "Volume Setting")
	float GetSfxVolume() const;
};
