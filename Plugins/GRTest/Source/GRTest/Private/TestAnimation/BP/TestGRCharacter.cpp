#include "TestAnimation/BP/TestGRCharacter.h"

#include "AbilitySystemComponent.h"
#include "TestAnimation/GA/TestGAEquipMontage.h"

#include "Camera/CameraComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "TestAnimation/BP/TestWeaponPickup.h"
#include "TestAnimation/DA/TestWeaponAsset.h"
#include "GameplayTagsManager.h"

ATestGRCharacter::ATestGRCharacter()
{
	AbilitySystem = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));

	WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMeshComponent->SetupAttachment(GetMesh());
}

UAbilitySystemComponent* ATestGRCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

void ATestGRCharacter::BeginPlay()
{
	Super::BeginPlay();

	AbilitySystem->InitAbilityActorInfo(this, this);

	FGameplayAbilitySpec Spec(UTestGAEquipMontage::StaticClass(), 1, 0);
	AbilitySystem->GiveAbility(Spec);
}

void ATestGRCharacter::EquipWeapon(ATestWeaponPickup* NewWeapon)
{
	if (!NewWeapon)
	{
		return;
	}

	UTestWeaponAsset* WeaponData = NewWeapon->GetWeaponData();
	if (!WeaponData)
	{
		return;
	}

	if (CurrentWeapon)
	{
		CurrentWeapon->Destroy();
		CurrentWeapon = nullptr;
	}

	CurrentWeaponAsset = WeaponData;
	CurrentWeapon = NewWeapon;
	bHasWeapon = true;

	const FGameplayTagContainer& TagContainer = WeaponData->WeaponTags;

	const FGameplayTag RifleTag = UGameplayTagsManager::Get().RequestGameplayTag(FName("Test.Weapon.Rifle"));
	const FGameplayTag SniperTag = UGameplayTagsManager::Get().RequestGameplayTag(FName("Test.Weapon.Sniper"));
	const FGameplayTag ShotgunTag = UGameplayTagsManager::Get().RequestGameplayTag(FName("Test.Weapon.Shotgun"));

	FName SocketName("RifleSocket");

	if (TagContainer.HasTagExact(SniperTag))
	{
		SocketName = FName("SniperSocket");
	}
	else if (TagContainer.HasTagExact(ShotgunTag))
	{
		SocketName = FName("ShotgunSocket");
	}

	if (USkeletalMeshComponent* CharMesh = GetMesh())
	{
		NewWeapon->AttachToComponent(CharMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);

		NewWeapon->SetActorScale3D(FVector(0.8f, 0.8f, 0.8f));
	}

	PushWeaponStateToAnimBP();

	if (AbilitySystem)
	{
		AbilitySystem->TryActivateAbilityByClass(UTestGAEquipMontage::StaticClass());
	}
}
