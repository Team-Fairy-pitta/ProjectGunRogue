#include "Weapon/GRWeaponInstance.h"

#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "Weapon/GRWeaponOptionPool.h"
#include "Weapon/GRWeaponDefinition.h"


FGRWeaponInstance::FGRWeaponInstance()
{
	bIsValid = 0;
	UpgradeLevel = 0;
	UpgradeDamage = 0.f;
	CurrentAmmo = 0;
	RerollCount = 0;
}

FGRWeaponInstance::FGRWeaponInstance(const FGRWeaponInstance& Other)
{
	bIsValid = Other.bIsValid;
	UpgradeLevel = Other.UpgradeLevel;
	UpgradeDamage = Other.UpgradeDamage;
	Options = Other.Options;
	AppliedEffects = Other.AppliedEffects;
	CurrentAmmo = Other.CurrentAmmo;
	CachedASC = nullptr;
	WeaponDefinition = nullptr;
	RerollCount = Other.RerollCount;
}

FGRWeaponInstance& FGRWeaponInstance::operator=(const FGRWeaponInstance& Other)
{
	this->bIsValid = Other.bIsValid;
	this->UpgradeLevel = Other.UpgradeLevel;
	this->UpgradeDamage = Other.UpgradeDamage;
	this->Options = Other.Options;
	this->AppliedEffects = Other.AppliedEffects;
	this->CurrentAmmo = Other.CurrentAmmo;
	this->CachedASC = nullptr;
	this->WeaponDefinition = nullptr;
	this->RerollCount = Other.RerollCount;
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
		UE_LOG(LogTemp, Display, TEXT("Weapon MaxLevel에 도달했습니다."));
		return;
	}

	UpgradeLevel++;
	UpgradeDamage += WeaponDefinition->UpgradeDamageIncrease;

	UE_LOG(LogTemp, Display, TEXT("Upgrade Level:%d"), UpgradeLevel);
	UE_LOG(LogTemp, Display, TEXT("Upgrade Damage:%f"), UpgradeDamage);

	if (UpgradeLevel % 3 == 0)
	{
		FWeaponOption NewOption = RandomOption();
		if (NewOption.EffectClass)
		{
			Options.Add(NewOption);

			ClearEffects();
			ApplyAllEffects();
		}
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

FWeaponOption FGRWeaponInstance::RandomOption() const
{
	FWeaponOption EmptyOption;

	if (!WeaponDefinition || !WeaponDefinition->OptionPool)
	{
		UE_LOG(LogTemp, Warning, TEXT("OptionPool 데이터가 없음"));
		return EmptyOption;
	}

	const TArray<FOptionPoolEntry>& Pool = WeaponDefinition->OptionPool->Options;

	if (Pool.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("OptionPool이 비어있음"));
		return EmptyOption;
	}

	const FOptionPoolEntry& Entry = Pool[FMath::RandRange(0, Pool.Num() - 1)];

	float RandomValue = FMath::FRandRange(Entry.MinValue, Entry.MaxValue);
	RandomValue = FMath::RoundToFloat(RandomValue * 10.0f) / 10.0f;

	FWeaponOption NewOption;
	NewOption.EffectClass = Entry.EffectClass;
	NewOption.Value = RandomValue;

	return NewOption;
}

void FGRWeaponInstance::RerollOption(int32 OptionSlotIndex)
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

	if (!Options.IsValidIndex(OptionSlotIndex))
	{
		UE_LOG(LogTemp, Warning, TEXT("Invalid OptionSlotIndex: %d"), OptionSlotIndex);
		return;
	}

	FWeaponOption NewOption = RandomOption();
	if (NewOption.EffectClass)
	{
		Options[OptionSlotIndex] = NewOption;
		RerollCount += 1;
		ClearEffects();
		ApplyAllEffects();
	}
}

void FGRWeaponInstance::AllRerollOption()
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
	int32 OptionNumber = Options.Num();
	Options.Empty();

	for (int32 i = 0; i < OptionNumber; ++i)
	{
		FWeaponOption NewOption = RandomOption();
		if (NewOption.EffectClass)
		{
			Options.Add(NewOption);
		}
	}
	RerollCount += 1;
	ClearEffects();
	ApplyAllEffects();
}

int32 FGRWeaponInstance::GetUpgradeCost() const
{
	return (GetLevel() + 1) * UpgradeCostPerLevel;
}

int32 FGRWeaponInstance::GetRerollCost() const
{
	return (RerollCount + 1) * RerollCostPerCount;
}

// 탄약 관련 구현
int32 FGRWeaponInstance::GetMaxAmmo() const
{
	return WeaponDefinition ? WeaponDefinition->MaxAmmo : 30;
}

bool FGRWeaponInstance::CheckCanReload() const
{
	const int32 MaxAmmoValue = GetMaxAmmo();
	return CurrentAmmo < MaxAmmoValue;
}

bool FGRWeaponInstance::ConsumeAmmo()
{
	if (CachedASC && CachedASC->GetOwnerRole() != ROLE_Authority)
	{
		return true;
	}

	if (CurrentAmmo <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[WeaponInstance] No ammo left!"));
		return false;
	}

	CurrentAmmo--;
	UE_LOG(LogTemp, Verbose, TEXT("[WeaponInstance] Ammo consumed: %d / %d"),
		CurrentAmmo, GetMaxAmmo());
	return true;
}

void FGRWeaponInstance::Reload()
{
	const int32 MaxAmmoValue = GetMaxAmmo();

	if (CurrentAmmo >= MaxAmmoValue)
	{
		UE_LOG(LogTemp, Warning, TEXT("[WeaponInstance] Already full: %d / %d"),
			CurrentAmmo, MaxAmmoValue);
		return;
	}

	const int32 OldAmmo = CurrentAmmo;
	CurrentAmmo = MaxAmmoValue;

	UE_LOG(LogTemp, Log, TEXT("[WeaponInstance] Reloaded: %d -> %d"),
		OldAmmo, CurrentAmmo);
}
