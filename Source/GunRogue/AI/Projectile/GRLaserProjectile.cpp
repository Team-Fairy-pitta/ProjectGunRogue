// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Projectile/GRLaserProjectile.h"
#include "Components/SphereComponent.h"
#include "AI/Character/GRAICharacter.h"
#include "Character/GRCharacter.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilitySystemComponent.h"

AGRLaserProjectile::AGRLaserProjectile()
	:Velocity(4000.0f),
	DamageGEClass(nullptr)
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("GRAIBossProjectile"));
	CollisionComponent->SetSimulatePhysics(false);
	CollisionComponent->SetNotifyRigidBodyCollision(true);
	RootComponent = CollisionComponent;
	
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->InitialSpeed = Velocity;
	ProjectileMovement->MaxSpeed = Velocity;
	ProjectileMovement->bRotationFollowsVelocity = true; // 방향 따라 회전
	ProjectileMovement->bShouldBounce = false;           
	ProjectileMovement->ProjectileGravityScale = 0.f;   
	ProjectileMovement->bAutoActivate = false;
}

void AGRLaserProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if (CollisionComponent)
		{
			CollisionComponent->OnComponentHit.AddDynamic(this, &AGRLaserProjectile::OnHit);
		}
	}
}

void AGRLaserProjectile::Launch(const FVector& NormalizeDirection)
{
	if(!HasAuthority())
	{
		return;
	}
	
	if (CollisionComponent)
	{
		ProjectileMovement->Velocity = NormalizeDirection * Velocity;
		ProjectileMovement->Activate();
	}
}

void AGRLaserProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                               FVector NormalImpulse, const FHitResult& Hit)
{
	if(!HasAuthority())
 	{
 		return;
 	}

	if (!OtherActor || OtherActor == this)
	{
		return;
	}
	
	if (OtherActor->IsA(AGRAICharacter::StaticClass()))
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	UAbilitySystemComponent* AIASC = GetInstigator()->FindComponentByClass<UAbilitySystemComponent>();
	if (!AIASC)
	{
		return;
	}

	if (!DamageGEClass)
	{
		return;
	}

	FGameplayEffectContextHandle BossEffectContext = AIASC->MakeEffectContext();
	BossEffectContext.AddInstigator(GetInstigator(), this); 
		
	FGameplayEffectSpecHandle BossSpecHandle = AIASC->MakeOutgoingSpec(DamageGEClass,1.f,BossEffectContext);
	

	AGRCharacter* PlayerChar=Cast<AGRCharacter>(OtherActor);
	if (PlayerChar)
	{
#if WITH_EDITOR
		FVector PlayerLoc = PlayerChar->GetActorLocation();
		if (GetWorld())
		{
			DrawDebugSphere(GetWorld(),PlayerLoc,20.f,12,FColor::Red,false,1.0f);
		}
#endif

		IAbilitySystemInterface* PlayerASI = Cast<IAbilitySystemInterface>(PlayerChar);
		if (!PlayerASI)
		{
			return;
		}
		
		UAbilitySystemComponent* PlayerASC = PlayerASI->GetAbilitySystemComponent();
		if (!PlayerASC)
		{
			return;
		}

		if (BossSpecHandle.IsValid())
		{
			AIASC->ApplyGameplayEffectSpecToTarget(*BossSpecHandle.Data.Get(),PlayerASC);
		}
	}

	Destroy();
}

