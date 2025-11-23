#include "GRWeaponBase.h"

#include "AbilitySystem/GRGameplayEffect.h"
#include "GRWeaponDataAsset.h"
#include "Character/GRCharacter.h"

#include "Components/BoxComponent.h"


AGRWeaponBase::AGRWeaponBase()
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicateMovement(true);

	PickupCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("PickupCollision"));
	PickupCollision->SetupAttachment(RootComponent);
	
}

void AGRWeaponBase::BeginPlay()
{
	Super::BeginPlay();
	
	if (WeaponData)
	{
		WeaponAbility.Damage = WeaponData->BaseDamage;
	}
	if (PickupCollision)
	{
		PickupCollision->OnComponentBeginOverlap.AddDynamic(this, &AGRWeaponBase::OnOverlap);
	}
	
}

void AGRWeaponBase::EquipWeapon(UAbilitySystemComponent* ASC)
{
	if (HasAuthority())
	{
		Server_EquipWeapon(ASC);
	}
	else
	{
		Server_EquipWeapon(ASC);
	}
}

void AGRWeaponBase::Server_EquipWeapon_Implementation(UAbilitySystemComponent* ASC)
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
	if (HasAuthority())
	{
		Server_UnequipWeapon();
	}
	else
	{
		Server_UnequipWeapon();
	}
}

void AGRWeaponBase::Server_UnequipWeapon_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("무기 해제"));

	ClearEffects();
	CachedASC = nullptr;
}

void AGRWeaponBase::TryUpgradeWeapon()
{
	if (HasAuthority())
	{
		Server_TryUpgradeWeapon();
	}
	else
	{
		Server_TryUpgradeWeapon();
	}
}

void AGRWeaponBase::Server_TryUpgradeWeapon_Implementation()
{
	if (!WeaponData || !WeaponData->OptionPool)
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponData, WeaponData->OptionPool이 없음"));
		return;
	}

	if (!(WeaponAbility.Level < WeaponData->MaxLevel))
	{
		UE_LOG(LogTemp, Warning, TEXT("Weapon MaxLevel에 도달했습니다."));
		return;
	}

	WeaponAbility.Level++;
	WeaponAbility.Damage += WeaponData->UpgradeDamageIncrease;


	if (WeaponAbility.Level % 3 == 0)
	{
		const auto& Pool = WeaponData->OptionPool->Options;
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

	return;
}

void AGRWeaponBase::ApplyAllEffects()
{
	if (!HasAuthority())
	{
		return;
	}

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
	if (!HasAuthority())
	{
		return;
	}

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
	if (!HasAuthority())
	{
		return;
	}

	if (AGRCharacter* Character = Cast<AGRCharacter>(OtherActor))
	{
		Character->NotifyWeaponOverlap(this);
	}
}
