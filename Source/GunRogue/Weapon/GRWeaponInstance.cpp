#include "Weapon/GRWeaponInstance.h"

#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "Weapon/GRWeaponDefinition.h"


FGRWeaponInstance::FGRWeaponInstance()
{
	// [NOTE] Replicate 테스트용 변수입니다. [TODO] 나중에 제거해야 합니다.
	Counter = 0;

	bIsValid = 0;
	UpgradeLevel = 0;
	UpgradeDamage = 0.f;
}

FGRWeaponInstance::FGRWeaponInstance(const FGRWeaponInstance& Other)
{
	// [NOTE] Replicate 테스트용 변수입니다. [TODO] 나중에 제거해야 합니다.
	Counter = Other.Counter;

	bIsValid = Other.bIsValid;
	UpgradeLevel = Other.UpgradeLevel;
	UpgradeDamage = Other.UpgradeDamage;
	Options = Other.Options;
	AppliedEffects = Other.AppliedEffects;
}

FGRWeaponInstance& FGRWeaponInstance::operator=(const FGRWeaponInstance& Other)
{
	// [NOTE] Replicate 테스트용 변수입니다. [TODO] 나중에 제거해야 합니다.
	this->Counter = Other.Counter;

	this->bIsValid = Other.bIsValid;
	this->UpgradeLevel = Other.UpgradeLevel;
	this->UpgradeDamage = Other.UpgradeDamage;
	this->Options = Other.Options;
	this->AppliedEffects = Other.AppliedEffects;
	return *this;
}

void FGRWeaponInstance::Init(UGRAbilitySystemComponent* ASC, UGRWeaponDefinition* InWeaponDefinition)
{
	CachedASC = ASC;
	WeaponDefinition = InWeaponDefinition;

	if (UpgradeLevel == 0)
	{
		UpgradeDamage = WeaponDefinition->BaseDamage;
	}
}

void FGRWeaponInstance::UpgradeWeapon()
{
	if (!WeaponDefinition || !WeaponDefinition->OptionPool)
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponData, WeaponData->OptionPool이 없음"));
		return;
	}

	if (CachedASC->GetOwnerRole() != ROLE_Authority)
	{
		return;
	}

	if (!(UpgradeLevel < WeaponDefinition->MaxLevel))
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon MaxLevel에 도달했습니다."));
		return;
	}

	UpgradeLevel++;
	UpgradeDamage += WeaponDefinition->UpgradeDamageIncrease;

	UE_LOG(LogTemp, Error, TEXT("Upgrade Level:%d"), UpgradeLevel);
	UE_LOG(LogTemp, Error, TEXT("Upgrade Damage:%f"), UpgradeDamage);

	if (UpgradeLevel % 3 == 0)
	{
		const auto& Pool = WeaponDefinition->OptionPool->Options;
		if (Pool.Num() == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("OptionPool이 없음"));
			return;
		}

		const FOptionPoolEntry& Entry = Pool[FMath::RandRange(0, Pool.Num() - 1)];


		float RandomValue = FMath::FRandRange(Entry.MinValue, Entry.MaxValue);

		RandomValue = FMath::RoundToFloat(RandomValue * 10.0f) / 10.0f;

		FWeaponOption NewOption;
		NewOption.EffectClass = Entry.EffectClass;
		NewOption.Value = RandomValue;

		Options.Add(NewOption);

		ClearEffects();
		ApplyAllEffects();
	}

	for(FWeaponOption& Option : Options)
	{
		UE_LOG(LogTemp, Display, TEXT("Option Effect: %s with Value: %f"),
			*Option.EffectClass->GetName(),
			Option.Value);
	}
}

void FGRWeaponInstance::ApplyAllEffects()
{
	if (!CachedASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("CachedASC가 없음"));
		return;
	}

	if (CachedASC->GetOwnerRole() != ROLE_Authority)
	{
		return;
	}

	AppliedEffects.Empty();

	for (const FWeaponOption& Option : Options)
	{
		if (!Option.EffectClass)
		{
			continue;
		}

		FGameplayEffectSpecHandle SpecHandle = CachedASC->MakeOutgoingSpec(
			Option.EffectClass,
			1.0f,
			CachedASC->MakeEffectContext()
		);

		if (!SpecHandle.IsValid())
		{
			continue;
		}

		FGameplayTag ValueTag = FGameplayTag::RequestGameplayTag("Data.OptionValue");

		SpecHandle.Data->SetSetByCallerMagnitude(ValueTag, Option.Value);

		FActiveGameplayEffectHandle Handle = CachedASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		AppliedEffects.Add(Handle);

	}
}

void FGRWeaponInstance::ClearEffects()
{
	if (!CachedASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("CachedASC가 없음"));
		return;
	}

	if (CachedASC->GetOwnerRole() != ROLE_Authority)
	{
		return;
	}

	for (FActiveGameplayEffectHandle& Handle : AppliedEffects)
	{
		if (Handle.IsValid())
		{
			CachedASC->RemoveActiveGameplayEffect(Handle);
		}
	}

	AppliedEffects.Empty();
}
