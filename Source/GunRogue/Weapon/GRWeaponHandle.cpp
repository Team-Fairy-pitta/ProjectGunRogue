#include "Weapon/GRWeaponHandle.h"
#include "Weapon/GRWeaponInstance.h"
#include "Weapon/GRWeaponDefinition.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"

void FGRWeaponHandle::EquipWeapon(UGRAbilitySystemComponent* ASC, UGRWeaponDefinition* InWeaponDefinition, const FGRWeaponInstance& InWeaponInstance)
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

	WeaponInstance = InWeaponInstance;

	WeaponInstance.Init(CachedASC, WeaponDefinition);
	WeaponInstance.ApplyAllEffects();

	UE_LOG(LogTemp, Display, TEXT("[FGRWeaponHandle] %s  WeaponInstance.CurrentLevel: %d"),
		*WeaponDefinition->WeaponName.ToString(),
		WeaponInstance.GetLevel());
	UE_LOG(LogTemp, Display, TEXT("[FGRWeaponHandle] %s  WeaponInstance.CurrentDamage: %f"),
		*WeaponDefinition->WeaponName.ToString(),
		WeaponInstance.GetDamage());

	UE_LOG(LogTemp, Display, TEXT("[FGRWeaponHandle] %s Ammo: %d / %d"),
		*WeaponDefinition->WeaponName.ToString(),
		WeaponInstance.GetCurrentAmmo(),
		WeaponInstance.GetMaxAmmo());


	// 슬롯에 저장만 하고 활성화는 별도로 호출
	UE_LOG(LogTemp, Display, TEXT("Weapon Equipped to slot: %s"),
		*WeaponDefinition->WeaponName.ToString());
}

void FGRWeaponHandle::UnequipWeapon()
{
	if (!IsValid(CachedASC))
	{
		UE_LOG(LogTemp, Error, TEXT("CachedASC is INVALID"));
		return;
	}

	if (bIsActive)
	{
		DeactivateWeapon();
	}

	if (WeaponDefinition)
	{
		UE_LOG(LogTemp, Display, TEXT("Weapon Unequipped: %s"),
			*WeaponDefinition->WeaponName.ToString());
	}

	WeaponDefinition = nullptr;
	WeaponInstance.ClearEffects();
	WeaponInstance.Invalidate();
	CachedASC = nullptr;
}

void FGRWeaponHandle::ActivateWeapon()
{
	if (bIsActive)
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon already active"));
		return;
	}

	if (!IsValid(CachedASC) || !IsValid(WeaponDefinition))
	{
		UE_LOG(LogTemp, Error, TEXT("Cannot activate weapon: Invalid ASC or Definition"));
		return;
	}

	// AbilitySet의 어빌리티와 이펙트 부여
	if (IsValid(WeaponDefinition->AbilitySet))
	{
		WeaponDefinition->AbilitySet->GiveToAbilitySystem(CachedASC, &GrantedHandles);
	}

	// 무기 장착 시 AttributeSet에 탄약 정보 로드 (UI 표시용)
	UGRCombatAttributeSet* CombatSet = const_cast<UGRCombatAttributeSet*>(
		CachedASC->GetSet<UGRCombatAttributeSet>()
		);
	if (CombatSet && WeaponInstance.IsValid())
	{
		CombatSet->UpdateAmmoDisplay(WeaponInstance.GetCurrentAmmo(), WeaponInstance.GetMaxAmmo());

		UE_LOG(LogTemp, Display, TEXT("[ActivateWeapon] %s - Ammo: %d/%d"),
			*WeaponDefinition->WeaponName.ToString(),
			WeaponInstance.GetCurrentAmmo(),
			WeaponInstance.GetMaxAmmo());
	}

	bIsActive = true;

	UE_LOG(LogTemp, Display, TEXT("Weapon Activated: %s"),
		*WeaponDefinition->WeaponName.ToString());
}

void FGRWeaponHandle::DeactivateWeapon()
{
	if (!bIsActive)
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon already inactive"));
		return;
	}

	if (!IsValid(CachedASC))
	{
		UE_LOG(LogTemp, Error, TEXT("CachedASC is INVALID"));
		return;
	}

	// 부여된 어빌리티/이펙트 제거
	GrantedHandles.TakeFromAbilitySystem(CachedASC);

	bIsActive = false;

	if (WeaponDefinition)
	{
		UE_LOG(LogTemp, Display, TEXT("Weapon Deactivated: %s"),
			*WeaponDefinition->WeaponName.ToString());
	}
}
