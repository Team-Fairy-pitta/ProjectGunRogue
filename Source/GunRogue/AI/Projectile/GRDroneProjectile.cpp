// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Projectile/GRDroneProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "AI/Character/GRAICharacter.h"
#include "Player/GRPlayerState.h"
#include "Character/GRCharacter.h"
#include "AI/Robot/AT/GRSkillAttributeSet_Robot.h"

AGRDroneProjectile::AGRDroneProjectile()
	:Velocity(3000.0f)
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(3.0f);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComp->SetCollisionResponseToChannel(ECC_EngineTraceChannel3,ECR_Ignore);
	RootComponent = CollisionComp;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = Velocity;
	ProjectileMovement->MaxSpeed = Velocity;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 0.f;   
	
	InitialLifeSpan = 5.0f;
}

void AGRDroneProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if (CollisionComp)
		{
			if (GetOwner())
			{
				CollisionComp->IgnoreActorWhenMoving(GetOwner(), true);
			}
			CollisionComp->OnComponentHit.AddDynamic(this, &AGRDroneProjectile::OnHit);
		}
	}
}

void AGRDroneProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	APawn* InstigatorPawn = GetInstigator();
	if (!InstigatorPawn)
	{
		return;
	}

	APlayerState* PS = InstigatorPawn->GetPlayerState();
	if (!PS)
	{
		return;
	}
	
	UAbilitySystemComponent* PlayerASC = PS->FindComponentByClass<UAbilitySystemComponent>();
	if (!PlayerASC)
	{
		return;
	}

	if (!DamageGEClass)
	{
		return;
	}

	FGameplayEffectContextHandle PlayerEffectContext = PlayerASC->MakeEffectContext();
	PlayerEffectContext.AddInstigator(GetInstigator()/*Instigator=Player*/, GetInstigator()/*Causer=Player*/);
		
	FGameplayEffectSpecHandle PlayerSpecHandle = PlayerASC->MakeOutgoingSpec(DamageGEClass,1.f,PlayerEffectContext);
	float Damage = GetFireDamageFromInstigator();
	PlayerSpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Ability.Damage.DroneFire")), Damage);
	
	AGRAICharacter* AIChar=Cast<AGRAICharacter>(OtherActor);
	if (AIChar)
	{
#if WITH_EDITOR
		FVector AILoc = AIChar->GetActorLocation();
		DrawDebugSphere(World,AILoc,20.f,12,FColor::Red,false,1.0f);
#endif

		IAbilitySystemInterface* AIASI = Cast<IAbilitySystemInterface>(AIChar);
		if (!AIASI)
		{
			return;
		}
		
		UAbilitySystemComponent* AIASC = AIASI->GetAbilitySystemComponent();
		if (!AIASC)
		{
			return;
		}

		if (PlayerSpecHandle.IsValid())
		{
			PlayerASC->ApplyGameplayEffectSpecToTarget(*PlayerSpecHandle.Data.Get(),AIASC);

			//Impact Cue
			FGameplayTag ImpactCueTag = FGameplayTag::RequestGameplayTag("GameplayCue.AI.Bullet.Impact");
			FGameplayCueParameters Params;
			Params.Location = Hit.Location;
			Params.Normal = Hit.Normal;
			PlayerASC->ExecuteGameplayCue(ImpactCueTag,Params);
		}
	}
	
	Destroy();
}

float AGRDroneProjectile::GetFireDamageFromInstigator() const
{
	AGRCharacter* GRCharacter = GetInstigator<AGRCharacter>();
	if (!IsValid(GRCharacter))
	{
		return 0.0f;
	}

	UAbilitySystemComponent* OwnerASC = GRCharacter->GetAbilitySystemComponent();
	if (!OwnerASC)
	{
		return 0.0f;
	}

	const UGRSkillAttributeSet_Robot* SkillAttributeSet = OwnerASC->GetSet<UGRSkillAttributeSet_Robot>();
	if (!SkillAttributeSet)
	{
		return 0.0f;
	}

	return SkillAttributeSet->GetDamage();
}



