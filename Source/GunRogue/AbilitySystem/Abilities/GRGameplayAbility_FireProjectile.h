#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GRGameplayAbility_FireWeapon.h"
#include "GRGameplayAbility_FireProjectile.generated.h"

UCLASS()
class GUNROGUE_API UGRGameplayAbility_FireProjectile : public UGRGameplayAbility_FireWeapon
{
	GENERATED_BODY()

protected:
	virtual void FireWeapon() override;

private:
	// 투사체 스폰 및 발사;
	void SpawnProjectile(const FVector& SpawnLocation, const FRotator& SpawnRotation);
};
