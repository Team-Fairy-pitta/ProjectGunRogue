#include "TestAnimation/BP/TestWeaponPickup.h"
#include "TestAnimation/DA/TestWeaponAsset.h"
#include "TestAnimation/GA/TestGAEquipMontage.h"
#include "TestAnimation/BP/TestGRCharacter.h"

#include "Components/SceneComponent.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"

#include "AbilitySystemBlueprintLibrary.h"   
#include "AbilitySystemComponent.h"             

ATestWeaponPickup::ATestWeaponPickup()
{
	PrimaryActorTick.bCanEverTick = false;

	PickupSphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	RootComponent = PickupSphere;

	PickupSphere->SetSphereRadius(60.f);
	PickupSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	WeaponMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	WeaponMeshComponent->SetupAttachment(PickupSphere);

	PickupSphere->OnComponentBeginOverlap.AddDynamic(this, &ATestWeaponPickup::OnPickupSphereBeginOverlap);
}

void ATestWeaponPickup::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	if (WeaponData && WeaponData->WeaponMesh) 
	{
		WeaponMeshComponent->SetSkeletalMesh(WeaponData->WeaponMesh);
	}
}

void ATestWeaponPickup::OnPickupSphereBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!WeaponData)
	{
		return;
	}

	ATestGRCharacter* Character = Cast<ATestGRCharacter>(OtherActor);

	Character->CurrentWeaponAsset = WeaponData;
	Character->bHasWeapon = true;
	Character->PushWeaponStateToAnimBP();

	UAbilitySystemComponent* ASC = Character->GetAbilitySystemComponent();
	ASC->TryActivateAbilityByClass(UTestGAEquipMontage::StaticClass());
}
