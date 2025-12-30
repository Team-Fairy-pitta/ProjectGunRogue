#include "AbilitySystem/Abilities/MeleeGA/GRBladeWaveProjectile.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/GRGameplayEffect.h"

#include "Character/GRCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"

AGRBladeWaveProjectile::AGRBladeWaveProjectile()
{
	bReplicates = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	Collision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Collision"));
	Collision->SetupAttachment(Root);
	Collision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	Collision->SetCollisionObjectType(ECC_WorldDynamic);
	Collision->SetCollisionResponseToAllChannels(ECR_Ignore);
	Collision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);

	Movement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("Movement"));
	Movement->InitialSpeed = 2200.0f;
	Movement->MaxSpeed = 2200.0f;
	Movement->bRotationFollowsVelocity = true;
	Movement->bShouldBounce = false;

	DataTag_Damage = FGameplayTag::RequestGameplayTag(TEXT("Attribute.Data.Damage"));
}

void AGRBladeWaveProjectile::InitProjectile(float InDamage, float InWaveScale, bool bInPierce)
{
	Damage = InDamage;
	WaveScale = InWaveScale;
	bPierce = bInPierce;
}

void AGRBladeWaveProjectile::BeginPlay()
{
	Super::BeginPlay();

	Collision->OnComponentBeginOverlap.AddDynamic(this, &AGRBladeWaveProjectile::OnOverlap);

	SetLifeSpan(LifeSeconds);

	Movement->ProjectileGravityScale = 0.0f;
	Movement->Velocity = GetActorForwardVector() * Movement->InitialSpeed;

	SetActorScale3D(FVector(WaveScale));
}

void AGRBladeWaveProjectile::OnOverlap(
	UPrimitiveComponent* OverlappedComp,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!OtherActor || OtherActor == this || OtherActor == GetOwner())
	{
		return;
	}

	if (OtherActor->IsA(AGRCharacter::StaticClass()))
	{
		Destroy();
		return;
	}

	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor);

	if (!TargetASC)
	{
		Destroy();
		return;
	}

	AActor* OwnerActor = GetOwner();
	if (!OwnerActor || !DamageEffect)
	{
		Destroy();
		return;
	}

	UAbilitySystemComponent* SourceASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OwnerActor);

	if (!SourceASC)
	{
		Destroy();
		return;
	}

	FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
	Context.AddSourceObject(this);
	Context.AddHitResult(SweepResult);

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffect, 1.0f, Context);

	if (SpecHandle.IsValid() && SpecHandle.Data.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(DataTag_Damage, Damage);
		SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
	}

	Destroy();
}
