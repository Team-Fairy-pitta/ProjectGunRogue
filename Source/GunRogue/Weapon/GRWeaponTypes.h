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
	None UMETA(DisplayName = "없음")
};
