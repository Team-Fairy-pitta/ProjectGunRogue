#pragma once

#include "GameFramework/CheatManager.h"
#include "GRCheatManager.generated.h"

/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRCheatManager : public UCheatManager
{
	GENERATED_BODY()
	

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

#pragma endregion
};
