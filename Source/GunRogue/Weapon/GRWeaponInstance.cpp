#include "Weapon/GRWeaponInstance.h"
#include "Weapon/GRWeaponDefinition.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"

void FGRWeaponInstance::EquipWeapon(UGRAbilitySystemComponent* ASC, UGRWeaponDefinition* InWeaponDefinition)
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

	// 1. 어빌리티 세트 부여 (있을 경우)
	if (IsValid(InWeaponDefinition->AbilitySet))
	{
		InWeaponDefinition->AbilitySet->GiveToAbilitySystem(CachedASC, &GrantedHandles);
	}

	// 2. 무기 스탯 GameplayEffect 적용
	if (InWeaponDefinition->WeaponStatsEffect)
	{
		FGameplayEffectContextHandle EffectContext = CachedASC->MakeEffectContext();
		EffectContext.AddSourceObject(CachedASC->GetOwner());

		FGameplayEffectSpecHandle SpecHandle = CachedASC->MakeOutgoingSpec(
			InWeaponDefinition->WeaponStatsEffect,
			1.0f,
			EffectContext
		);

		if (SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle ActiveHandle = CachedASC->ApplyGameplayEffectSpecToSelf(
				*SpecHandle.Data.Get()
			);

			ActiveEffectHandles.Add(ActiveHandle);
		}
	}

	// 3. 추가 이펙트들 적용
	for (TSubclassOf<UGameplayEffect> EffectClass : InWeaponDefinition->WeaponEffects)
	{
		if (EffectClass)
		{
			FGameplayEffectContextHandle EffectContext = CachedASC->MakeEffectContext();
			EffectContext.AddSourceObject(CachedASC->GetOwner());

			FGameplayEffectSpecHandle SpecHandle = CachedASC->MakeOutgoingSpec(
				EffectClass,
				1.0f,
				EffectContext
			);

			if (SpecHandle.IsValid())
			{
				FActiveGameplayEffectHandle ActiveHandle = CachedASC->ApplyGameplayEffectSpecToSelf(
					*SpecHandle.Data.Get()
				);

				ActiveEffectHandles.Add(ActiveHandle);
			}
		}
	}

	UE_LOG(LogTemp, Display, TEXT("Weapon Equipped: %s"),
		*WeaponDefinition->WeaponName.ToString());
}

void FGRWeaponInstance::UnequipWeapon()
{
	if (!IsValid(CachedASC))
	{
		UE_LOG(LogTemp, Error, TEXT("CachedASC is INVALID"));
		return;
	}

	// 부여된 어빌리티/이펙트 제거
	GrantedHandles.TakeFromAbilitySystem(CachedASC);

	// GameplayEffect 제거
	for (const FActiveGameplayEffectHandle& Handle : ActiveEffectHandles)
	{
		if (Handle.IsValid())
		{
			CachedASC->RemoveActiveGameplayEffect(Handle);
		}
	}
	ActiveEffectHandles.Empty();

	if (WeaponDefinition)
	{
		UE_LOG(LogTemp, Display, TEXT("Weapon Unequipped: %s"), *WeaponDefinition->WeaponName.ToString());
	}

	WeaponDefinition = nullptr;
	CachedASC = nullptr;
}
