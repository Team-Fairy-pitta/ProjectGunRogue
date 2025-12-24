#pragma once

#include "CoreMinimal.h"
#include "GRWeaponTypes.generated.h"

// 무기 카테고리 열거형
UENUM(BlueprintType)
enum class EGRWeaponCategory : uint8
{
	Pistol UMETA(DisplayName = "권총"),
	Shotgun UMETA(DisplayName = "산탄총"),
	Rifle UMETA(DisplayName = "소총"),
	SniperRifle UMETA(DisplayName = "저격 소총"),
	ExplosiveLauncher UMETA(DisplayName = "폭발 무기"),
	None UMETA(DisplayName = "없음")
};

// 무기 발사 타입
UENUM(BlueprintType)
enum class EGRWeaponFireType : uint8
{
	Hitscan UMETA(DisplayName = "히트스캔"),
	Projectile UMETA(DisplayName = "투사체")
};
