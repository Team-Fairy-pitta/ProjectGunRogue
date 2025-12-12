#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/GRGameplayAbility_FireWeapon.h"
#include "GRGameplayAbility_HitscanAttack.generated.h"

class UGameplayEffect;

/**
 * 히트스캔 무기 (라이플, SMG 등)
 */

UCLASS()
class GUNROGUE_API UGRGameplayAbility_HitscanAttack : public UGRGameplayAbility_FireWeapon
{
	GENERATED_BODY()


protected:

	virtual void FireWeapon() override;

private:

	// Muzzle에서 타겟으로 라인 트레이스 (히트스캔 전용)
	bool TraceFromMuzzle(const FVector& MuzzleLoc, const FVector& TargetPoint, FHitResult& OutHit);

	// 히트 처리 (히트스캔 전용 - 즉시 데미지)
	void HandleHit(const FHitResult& Hit);
};
