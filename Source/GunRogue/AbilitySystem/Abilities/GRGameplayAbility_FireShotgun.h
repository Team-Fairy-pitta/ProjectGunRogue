#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GRGameplayAbility_HitscanAttack.h"
#include "GRGameplayAbility_FireShotgun.generated.h"

/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRGameplayAbility_FireShotgun : public UGRGameplayAbility_HitscanAttack
{
	GENERATED_BODY()

public:
	virtual void FireWeapon() override;

private:
	// 단일 펠렛 발사
	void FireSinglePellet(
		const FVector& MuzzleLocation,
		const FVector& BaseTargetPoint,
		const FVector& CameraLocation,
		const FRotator& BaseCameraRotation,
		float PelletSpreadAngle);

	// 펠렛별 랜덤 스프레드 적용
	FVector ApplyPelletSpread(const FVector& BaseDirection, float SpreadAngle);
	
};
