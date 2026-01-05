#include "Weapon/GRWeaponInstance.h"

#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"
#include "Weapon/GRWeaponOptionPool.h"
#include "Weapon/GRWeaponDefinition.h"


FGRWeaponInstance::FGRWeaponInstance()
{
	bIsValid = 0;
	UpgradeLevel = 0;
	CurrentAmmo = 0;
	RerollCount = 0;
}

FGRWeaponInstance::FGRWeaponInstance(const FGRWeaponInstance& Other)
{
	bIsValid = Other.bIsValid;
	UpgradeLevel = Other.UpgradeLevel;
	Options = Other.Options;
	AppliedEffects = Other.AppliedEffects;
	CurrentAmmo = Other.CurrentAmmo;
	CachedASC = nullptr;
	WeaponDefinition = nullptr;
	RerollCount = Other.RerollCount;

	CachedTotalDamage = Other.CachedTotalDamage;
	CachedTotalWeakMultuplier = Other.CachedTotalWeakMultuplier;
	CachedTotalFireRate = Other.CachedTotalFireRate;
	CachedTotalMagazine = Other.CachedTotalMagazine;
}

FGRWeaponInstance& FGRWeaponInstance::operator=(const FGRWeaponInstance& Other)
{
	this->bIsValid = Other.bIsValid;
	this->UpgradeLevel = Other.UpgradeLevel;
	this->Options = Other.Options;
	this->AppliedEffects = Other.AppliedEffects;
	this->CurrentAmmo = Other.CurrentAmmo;
	this->CachedASC = nullptr;
	this->WeaponDefinition = nullptr;
	this->RerollCount = Other.RerollCount;

	this->CachedTotalDamage = Other.CachedTotalDamage;
	this->CachedTotalWeakMultuplier = Other.CachedTotalWeakMultuplier;
	this->CachedTotalFireRate = Other.CachedTotalFireRate;
	this->CachedTotalMagazine = Other.CachedTotalMagazine;

	return *this;
}

void FGRWeaponInstance::Init(UGRAbilitySystemComponent* ASC, UGRWeaponDefinition* InWeaponDefinition)
{
	CachedASC = ASC;
	WeaponDefinition = InWeaponDefinition;
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

	UE_LOG(LogTemp, Display, TEXT("Upgrade Level:%d"), UpgradeLevel);

	if (UpgradeLevel % 3 == 0)
	{
		FWeaponOption NewOption = RandomOption();
		if (NewOption.EffectClass)
		{
			Options.Add(NewOption);
		}
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


	// 기본 데미지 효과 적용
	if (WeaponDefinition && WeaponDefinition->BaseDamageEffect)
	{
		FGameplayEffectSpecHandle SpecHandle =
			CachedASC->MakeOutgoingSpec(
				WeaponDefinition->BaseDamageEffect,
				1.f,
				CachedASC->MakeEffectContext()
			);

		float AdditiveDamage = UpgradeLevel * WeaponDefinition->UpgradeDamageIncrease;

		SpecHandle.Data->SetSetByCallerMagnitude(
			FGameplayTag::RequestGameplayTag(TEXT("Weapon.BaseDamage")),
			AdditiveDamage
		);

		FActiveGameplayEffectHandle Handle = CachedASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		AppliedEffects.Add(Handle);
	}

	// 옵션 효과 적용
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

		for (const FOptionItem& OptionItem : Option.OptionItems)
		{
			SpecHandle.Data->SetSetByCallerMagnitude(
				OptionItem.OptionTag,
				OptionItem.Value
			);
		}

		FActiveGameplayEffectHandle Handle = CachedASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		AppliedEffects.Add(Handle);

	}

	UpdateCachedAttributes();
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

	EmptyOption.EffectClass = Entry.EffectClass;

	for (const FOptionValueRange& Range : Entry.ValueRanges)
	{
		float Value = FMath::FRandRange(Range.ValueRange.Min, Range.ValueRange.Max);
		Value = FMath::RoundToFloat(Value * 10.f) / 10.f;

		EmptyOption.OptionItems.Add({ Range.DataTag, Value });
		EmptyOption.bIsPercentValue = Range.bIsPercentValue;
		EmptyOption.bIsAdditivePercent = Range.bIsAdditivePercent;
	}

	return EmptyOption;
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
}

float FGRWeaponInstance::GetTotalDamage() const
{
	return CachedTotalDamage;
}

float FGRWeaponInstance::GetTotalWeakMultuplier() const
{
	return CachedTotalWeakMultuplier;
}

float FGRWeaponInstance::GetTotalFireRate() const
{
	return CachedTotalFireRate;
}

float FGRWeaponInstance::GetTotalMagazine() const
{
	return CachedTotalMagazine;
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
	if (CachedASC)
	{
		const UGRCombatAttributeSet* CombatSet = CachedASC->GetSet<UGRCombatAttributeSet>();
		if (CombatSet)
		{
			return CombatSet->GetMaxAmmo();
		}
	}

	UE_LOG(LogTemp, Error, TEXT("CANNOT FIND UGRCombatAttributeSet"));
	return -1;
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

void FGRWeaponInstance::UpdateCachedAttributes()
{
	if (!CachedASC)
	{
		UE_LOG(LogTemp, Error, TEXT("CachedASC is INVALID"));
		return;
	}

	const UGRCombatAttributeSet* CombatAttributeSet = Cast<UGRCombatAttributeSet>(CachedASC->GetAttributeSet(UGRCombatAttributeSet::StaticClass()));
	if (!CombatAttributeSet)
	{
		UE_LOG(LogTemp, Error, TEXT("UGRCombatAttributeSet is INVALID"));
		return;
	}

	CachedTotalDamage = CombatAttributeSet->CalculateWeaponDamage();
	CachedTotalWeakMultuplier = CombatAttributeSet->CalculateCriticalMultiplier(true /*bIsCritical*/);
	CachedTotalFireRate = CombatAttributeSet->GetFireRate();
	CachedTotalMagazine = CombatAttributeSet->GetMaxAmmo();
}
