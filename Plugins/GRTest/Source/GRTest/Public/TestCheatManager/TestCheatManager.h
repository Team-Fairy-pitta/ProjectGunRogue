#pragma once

#include "GameFramework/CheatManager.h"
#include "Editor.h"
#include "TestCheatManager.generated.h"


class UTestItemDataAsset_Cheat;
class UTestLocationDataAsset;

/**
 * 
 */
UCLASS()
class GRTEST_API UTestCheatManager : public UCheatManager
{
	GENERATED_BODY()

#pragma region DataAsset

public:
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	TObjectPtr<UTestItemDataAsset_Cheat> ItemDataAsset;

	UPROPERTY(EditDefaultsOnly, Category = "Location")
	TObjectPtr<UTestLocationDataAsset> LocationDataAsset;

#pragma endregion

#pragma region Exec

protected:
	UFUNCTION(exec)
	void Flying();

	UFUNCTION(exec)
	void Walking();

	UFUNCTION(exec)
	void SelfDamage(float DamageAmount);

	UFUNCTION(exec)
	void SelfHeal(float HealAmount);

	UFUNCTION(exec)
	void SpawnItem(const FString& ItemName, float Distance);

	UFUNCTION(exec)
	void SI(const FString& ItemName, float Distance)
	{
		SpawnItem(ItemName, Distance);
	}

	UFUNCTION(exec)
	void LocationTeleport(const FString& LocationName);

	UFUNCTION(exec)
	void TP(const FString& LocationName)
	{
		LocationTeleport(LocationName);
	}

	UFUNCTION(exec)
	void SaveCurrentLocation(const FName LocationName);

#pragma endregion

};
