#pragma once

#include "UObject/NoExportTypes.h"
#include "GRWeaponInstance.generated.h"


// 무기의 런타임 정보를 기록하는 구조체
// 강화 정보와 가지고 있는 옵션 정보를 저장한다.
// 무기를 장착하면 PlayerState에 있는 WeaponHandle에 복사된다.
// 무기를 버리면 WeaponActor에 복사된다.
USTRUCT(BlueprintType)
struct GUNROGUE_API FGRWeaponInstance
{
	GENERATED_BODY()

public:
	FGRWeaponInstance();
	FGRWeaponInstance(const FGRWeaponInstance& Other);
	FGRWeaponInstance& operator=(const FGRWeaponInstance& Other);

	void Validate() { bIsValid = 1; }
	void Invalidate() { bIsValid = 0; }
	bool IsValid() const { return bIsValid == 1; }

	// [NOTE] Replicate 테스트용 변수입니다. [TODO] 나중에 제거해야 합니다.
	UPROPERTY()
	int32 Counter;

protected:
	UPROPERTY()
	int32 UpgradeLevel;

	UPROPERTY();
	int8 bIsValid;
};
