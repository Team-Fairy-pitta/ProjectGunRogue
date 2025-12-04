// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Projectile/GRLaserProjectile.h"
#include "Components/SphereComponent.h"
#include "AI/Character/GRLuwoAICharacter.h"
#include "Character/GRCharacter.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/DamageType.h"
#include "GameFramework/ProjectileMovementComponent.h"

AGRLaserProjectile::AGRLaserProjectile()
	:DamageAmount(100.0f)
	,Velocity(4000.0f)
{
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;
	SetReplicateMovement(true);

	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("AIProjectile"));
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
	
	//UE_LOG(LogTemp, Warning, TEXT("AGRRockProjectile::OnHit : Other Actor : %s"),*OtherActor->GetName());
	
	if (OtherActor->IsA(AGRLuwoAICharacter::StaticClass()))
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	AGRCharacter* PlayerChar=Cast<AGRCharacter>(OtherActor);
	if (PlayerChar)
	{
		UGameplayStatics::ApplyDamage(PlayerChar, DamageAmount,GetInstigatorController() , this, UDamageType::StaticClass());

#if WITH_EDITOR
		FVector PlayerLoc = PlayerChar->GetActorLocation();
		if (GetWorld())
		{
			DrawDebugSphere(GetWorld(),PlayerLoc,20.f,12,FColor::Red,false,1.0f);
		}
#endif
	}

	Destroy();
}

