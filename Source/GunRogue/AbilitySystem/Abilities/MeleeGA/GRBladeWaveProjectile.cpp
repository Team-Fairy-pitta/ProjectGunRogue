#include "AbilitySystem/Abilities/MeleeGA/GRBladeWaveProjectile.h"

#include "AbilitySystemGlobals.h"
#include "AbilitySystemComponent.h"
#include "Net/UnrealNetwork.h"

#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/GRGameplayEffect.h"
#include "AbilitySystem/Attributes/GRSkillAttributeSet_MeleeSkill.h"

#include "Character/GRCharacter.h"

#include "Components/CapsuleComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Engine/World.h"

AGRBladeWaveProjectile::AGRBladeWaveProjectile()
{
	bReplicates = true;
	SetReplicateMovement(true);

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
	Movement->ProjectileGravityScale = 0.0f;

	DataTag_Damage = FGameplayTag::RequestGameplayTag(TEXT("Attribute.Data.Damage"));
}

void AGRBladeWaveProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGRBladeWaveProjectile, Damage);
	DOREPLIFETIME(AGRBladeWaveProjectile, WaveScale);
	DOREPLIFETIME(AGRBladeWaveProjectile, bPierce);
}

void AGRBladeWaveProjectile::InitProjectile(float InDamage, float InWaveScale, bool bInPierce)
{
	if (HasAuthority())
	{
		AGRCharacter* OwnerChar = Cast<AGRCharacter>(GetOwner());
		if (OwnerChar)
		{
			UGRAbilitySystemComponent* SourceASC = Cast<UGRAbilitySystemComponent>(
				UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerChar));

			if (SourceASC)
			{
				const UGRSkillAttributeSet_MeleeSkill* SkillSet = SourceASC->GetSet<UGRSkillAttributeSet_MeleeSkill>();
				if (SkillSet)
				{
					const float MultScale = SkillSet->GetBladeWave_BaseWaveScaleMultiplier();
					WaveScale *= MultScale;
				}
			}
		}

		SetActorScale3D(FVector(WaveScale));
		ForceNetUpdate();
	}
}

void AGRBladeWaveProjectile::BeginPlay()
{
	Super::BeginPlay();

	Collision->OnComponentBeginOverlap.AddDynamic(this, &AGRBladeWaveProjectile::OnOverlap);
	SetLifeSpan(LifeSeconds);

	if (HasAuthority())
	{
		if (Damage <= 0.f)
		{
			ComputeParametersOnServer();
		}

		SetActorScale3D(FVector(WaveScale));

		Movement->Velocity = GetActorForwardVector() * Movement->InitialSpeed;

		ForceNetUpdate();
	}
}

void AGRBladeWaveProjectile::OnRep_WaveScale()
{
	SetActorScale3D(FVector(WaveScale));
}

void AGRBladeWaveProjectile::ComputeParametersOnServer()
{
	AGRCharacter* OwnerChar = Cast<AGRCharacter>(GetOwner());
	if (!OwnerChar)
	{
		return;
	}

	UGRAbilitySystemComponent* SourceASC = Cast<UGRAbilitySystemComponent>(
		UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerChar));

	if (!SourceASC)
	{
		return;
	}

	const UGRSkillAttributeSet_MeleeSkill* SkillSet = SourceASC->GetSet<UGRSkillAttributeSet_MeleeSkill>();
	if (!SkillSet)
	{
		return;
	}

	Damage = SkillSet->GetBladeWave_BaseDamage();

	const float BaseScale = SkillSet->GetBladeWave_BaseWaveScale();
	const float MultScale = SkillSet->GetBladeWave_BaseWaveScaleMultiplier();

	WaveScale = BaseScale * MultScale;
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
		return;
	}


	UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OtherActor);
	if (!TargetASC)
	{
		if (!bPierce)
		{
			Destroy();
		}
		return;
	}

	AActor* SourceActor = GetOwner();
	UGRAbilitySystemComponent* SourceASC = Cast<UGRAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(SourceActor));

	if (!SourceASC || !DamageEffect)
	{
		if (!bPierce)
		{
			Destroy();
		}
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

	if (!bPierce)
	{
		Destroy();
	}
}
