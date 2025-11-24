#include "Weapon/GRWeaponInstance.h"

FGRWeaponInstance::FGRWeaponInstance()
{
	// [NOTE] Replicate 테스트용 변수입니다. [TODO] 나중에 제거해야 합니다.
	Counter = 0;

	bIsValid = 0;
	UpgradeLevel = 0;
}

FGRWeaponInstance::FGRWeaponInstance(const FGRWeaponInstance& Other)
{
	// [NOTE] Replicate 테스트용 변수입니다. [TODO] 나중에 제거해야 합니다.
	Counter = Other.Counter;

	bIsValid = Other.bIsValid;
	UpgradeLevel = Other.UpgradeLevel;
}

FGRWeaponInstance& FGRWeaponInstance::operator=(const FGRWeaponInstance& Other)
{
	// [NOTE] Replicate 테스트용 변수입니다. [TODO] 나중에 제거해야 합니다.
	this->Counter = Other.Counter;

	this->bIsValid = Other.bIsValid;
	this->UpgradeLevel = Other.UpgradeLevel;
	return *this;
}
