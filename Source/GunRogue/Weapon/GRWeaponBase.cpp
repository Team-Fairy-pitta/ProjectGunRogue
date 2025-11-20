#include "GRWeaponBase.h"

#include "AbilitySystem/GRGameplayEffect.h"
#include "GRWeaponDataAsset.h"
#include "Character/GRCharacter.h"

#include "Components/BoxComponent.h"


AGRWeaponBase::AGRWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;

	PickupCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("PickupCollision"));
	PickupCollision->SetupAttachment(RootComponent);
	
}

void AGRWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (WeaponData)
	{
		WeaponAbllity.Damage = WeaponData->BaseDamage;
	}
	if (PickupCollision)
	{
		PickupCollision->OnComponentBeginOverlap.AddDynamic(this, &AGRWeaponBase::OnOverlap);
	}
	
}

void AGRWeaponBase::EquipWeapon(UAbilitySystemComponent* ASC)
{
	if (CachedASC)
	{
		return;
	}

	CachedASC = ASC;

	UE_LOG(LogTemp, Warning, TEXT("무기 장착"));

	ClearEffects();
	ApplyAllEffects();
}

void AGRWeaponBase::UnequipWeapon()
{
	UE_LOG(LogTemp, Warning, TEXT("무기 해제"));

	ClearEffects();
	CachedASC = nullptr;
}

bool AGRWeaponBase::TryUpgradeWeapon()
{
	if (!WeaponData || !WeaponData->OptionPool)
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponData, WeaponData->OptionPool이 없음"));
		return false;
	}

	if (!(WeaponAbllity.Level <= WeaponData->MaxLevel))
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon MaxLevel에 도달했습니다."));
		return false;
	}

	WeaponAbllity.Level++;
	WeaponAbllity.Damage += WeaponData->UpgradeDamageIncrease;


	if(WeaponAbllity.Level % 3 == 0)
	{
		const auto& Pool = WeaponData->OptionPool->Options;
		if (Pool.Num() == 0)
		{
			UE_LOG(LogTemp, Warning, TEXT("OptionPool이 없음"));
			return true;
		}

		const FOptionPoolEntry& Entry = Pool[FMath::RandRange(0, Pool.Num() - 1)];


		float RandomValue = FMath::FRandRange(Entry.MinValue, Entry.MaxValue);

		FWeaponOption NewOption;
		NewOption.EffectClass = Entry.EffectClass;
		NewOption.Value = RandomValue;

		Options.Add(NewOption);

		ClearEffects();
		ApplyAllEffects();
	}

	return true;
}

void AGRWeaponBase::ApplyAllEffects()
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

void AGRWeaponBase::ClearEffects()
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

void AGRWeaponBase::OnOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (AGRCharacter* Character = Cast<AGRCharacter>(OtherActor))
	{
		Character->NotifyWeaponOverlap(this);
	}
}
