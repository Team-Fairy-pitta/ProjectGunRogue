#include "AbilitySystem/Abilities/MeleeGA/GRBladeWaveProjectile.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/GRGameplayEffect.h"

#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "TimerManager.h"
#include "Engine/World.h"

AGRBladeWaveProjectile::AGRBladeWaveProjectile()
{
	bReplicates = true;

	Collision = CreateDefaultSubobject<USphereComponent>(TEXT("Collision"));
	SetRootComponent(Collision);
	Collision->InitSphereRadius(18.0f);
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

void AGRBladeWaveProjectile::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!OtherActor || OtherActor == GetOwner())
	{
		return;
	}

	if (!HasAuthority())
	{
		return;
	}

	ApplyDamageTo(OtherActor, SweepResult);

	if (!bPierce)
	{
		Destroy();
	}
}

void AGRBladeWaveProjectile::ApplyDamageTo(AActor* TargetActor, const FHitResult& Hit) const
{
	if (!TargetActor || !DamageEffect) return;

	AActor* OwnerActor = GetOwner();
	if (!OwnerActor) return;

	UAbilitySystemComponent* SourceASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerActor);
	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(TargetActor);
	if (!SourceASC || !TargetASC) return;

	FGameplayEffectContextHandle Ctx = SourceASC->MakeEffectContext();
	Ctx.AddSourceObject(this);
	Ctx.AddHitResult(Hit);

	FGameplayEffectSpecHandle Spec = SourceASC->MakeOutgoingSpec(DamageEffect, 1.0f, Ctx);
	if (!Spec.IsValid()) return;

	Spec.Data->SetSetByCallerMagnitude(DataTag_Damage, Damage);

	SourceASC->ApplyGameplayEffectSpecToTarget(*Spec.Data.Get(), TargetASC);
}

