#include "Weapon/GRWeaponHandle.h"
#include "Weapon/GRWeaponDefinition.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"

void FGRWeaponHandle::EquipWeapon(UGRAbilitySystemComponent* ASC, UGRWeaponDefinition* InWeaponDefinition)
{
	if (!IsValid(ASC))
	{
		UE_LOG(LogTemp, Error, TEXT("ASC is INVALID"));
		return;
	}

	if (!IsValid(InWeaponDefinition))
	{
		UE_LOG(LogTemp, Error, TEXT("WeaponDefinition is INVALID"));
		return;
	}

	CachedASC = ASC;
	WeaponDefinition = InWeaponDefinition;

	// 어빌리티 세트 부여
	if (IsValid(InWeaponDefinition->AbilitySet))
	{
		InWeaponDefinition->AbilitySet->GiveToAbilitySystem(CachedASC, &GrantedHandles);

		UE_LOG(LogTemp, Display, TEXT("Weapon Equipped: %s"),
			*WeaponDefinition->WeaponName.ToString());
	}
}

void FGRWeaponHandle::UnequipWeapon()
{
	if (!IsValid(CachedASC))
	{
		UE_LOG(LogTemp, Error, TEXT("CachedASC is INVALID"));
		return;
	}

	// 부여된 어빌리티/이펙트 제거
	GrantedHandles.TakeFromAbilitySystem(CachedASC);

	if (WeaponDefinition)
	{
		UE_LOG(LogTemp, Display, TEXT("Weapon Unequipped: %s"), *WeaponDefinition->WeaponName.ToString());
	}

	WeaponDefinition = nullptr;
	CachedASC = nullptr;
}
