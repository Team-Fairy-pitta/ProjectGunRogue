#include "GRWeaponBase.h"

#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"
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

void AGRWeaponBase::EquipWeapon(AGRCharacter* Character)
{
	CharacterASC = Character->GetAbilitySystemComponent();

	UE_LOG(LogTemp, Warning, TEXT("무기 장착"));

	ClearEffects();
	ApplyAllEffects();
}

void AGRWeaponBase::UnequipWeapon()
{
	UE_LOG(LogTemp, Warning, TEXT("무기 해제"));

	ClearEffects();
	CharacterASC = nullptr;
}

bool AGRWeaponBase::TryUpgradeWeapon()
{
	if (!WeaponData || !WeaponData->OptionPool)
	{
		UE_LOG(LogTemp, Warning, TEXT("WeaponData, WeaponData->OptionPool이 없음"));
		return false;
	}

	WeaponAbllity.Level++;
	WeaponAbllity.Damage += WeaponData->UpgradeDamageIncrease;


	if(WeaponAbllity.Level % 3 == 0)
	{
		const auto& Pool = WeaponData->OptionPool->Options;
		if (Pool.Num() == 0)
		{
			return true;
		}

		const FOptionPoolEntry& Entry = Pool[FMath::RandRange(0, Pool.Num() - 1)];


		float RandomValue = FMath::FRandRange(Entry.MinValue, Entry.MaxValue);

		FWeaponOption NewOption;
		NewOption.EffectClass = Entry.EffectClass;
		NewOption.Value = RandomValue;

		Options.Add(NewOption);

		// ASC에 효과 재적용
		ClearEffects();
		ApplyAllEffects();
	}
	ApplyDamage();
	UE_LOG(LogTemp, Warning, TEXT("깅화"));

	return true;
}

void AGRWeaponBase::ApplyDamage()
{
	if (!CharacterASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterASC가 없음"));
		return;
	}

	CharacterASC->ApplyModToAttribute(
		UGRCombatAttributeSet::GetWeaponDamage_AdditiveAttribute(),
		EGameplayModOp::Additive,
		WeaponAbllity.Damage
	);
}

void AGRWeaponBase::ApplyAllEffects()
{
	if (!CharacterASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterASC가 없음"));
		return;
	}

	AppliedEffects.Empty();

	for (const auto& Option : Options)
	{
		if (!Option.EffectClass)
		{
			continue;
		}
		
		// 옵션 값 적용

		UE_LOG(LogTemp, Warning, TEXT("옵션 적용"));

		// GameplayEffectSpec 생성
		FGameplayEffectSpecHandle SpecHandle = CharacterASC->MakeOutgoingSpec(
			Option.EffectClass,
			1.0f,
			CharacterASC->MakeEffectContext()
		);

		if (!SpecHandle.IsValid())
		{
			continue;
		}

		// SetByCaller 적용하기 위해 태그 생성
		FGameplayTag ValueTag = FGameplayTag::RequestGameplayTag("Data.OptionValue");

		// 값 부여
		SpecHandle.Data->SetSetByCallerMagnitude(ValueTag, Option.Value);

		// 플레이어에게 적용
		FActiveGameplayEffectHandle Handle = CharacterASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

		AppliedEffects.Add(Handle);

	}
}

void AGRWeaponBase::ClearEffects()
{
	if (!CharacterASC)
	{
		UE_LOG(LogTemp, Warning, TEXT("CharacterASC가 없음"));
		return;
	}

	for (auto& Handle : AppliedEffects)
	{
		CharacterASC->RemoveActiveGameplayEffect(Handle);
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



