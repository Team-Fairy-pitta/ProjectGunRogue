#include "Weapon/GRWeaponInstance.h"

#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "Weapon/GRWeaponDefinition.h"


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

void FGRWeaponInstance::Init(UGRAbilitySystemComponent* ASC, UGRWeaponDefinition* InWeaponDefinition)
{
	CachedASC = ASC;
	WeaponDefinition = InWeaponDefinition;

	if (!CurrentDamage)
	{
		CurrentDamage = WeaponDefinition->BaseDamage;
	}
}

void FGRWeaponInstance::WeaponUpgrade()
{
	if (!WeaponDefinition || !WeaponDefinition->OptionPool)
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponData, WeaponData->OptionPool이 없음"));
		return;
	}

	if (!(CurrentLevel < WeaponDefinition->MaxLevel))
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon MaxLevel에 도달했습니다."));
		return;
	}

	CurrentLevel++;
	CurrentDamage += WeaponDefinition->UpgradeDamageIncrease;


	if (CurrentLevel % 3 == 0)
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
}

void FGRWeaponInstance::ApplyAllEffects()
{
	if (!CachedASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("CachedASC가 없음"));
		return;
	}

	AppliedEffects.Empty();

	for (const auto& Option : Options)
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

	for (auto& Handle : AppliedEffects)
	{
		CachedASC->RemoveActiveGameplayEffect(Handle);
	}

	AppliedEffects.Empty();
}
