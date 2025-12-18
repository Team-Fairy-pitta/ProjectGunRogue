// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Projectile/GRAIBulletProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "Character/GRCharacter.h"

AGRAIBulletProjectile::AGRAIBulletProjectile()
	:Velocity(3000.0f)
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;

	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(3.0f);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
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

void AGRAIBulletProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if (CollisionComp)
		{
			CollisionComp->OnComponentHit.AddDynamic(this, &AGRAIBulletProjectile::OnHit);
		}
	}
}

void AGRAIBulletProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
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

	FGameplayEffectContextHandle AIEffectContext = AIASC->MakeEffectContext();
	AIEffectContext.AddInstigator(GetInstigator(), this); 
		
	FGameplayEffectSpecHandle AISpecHandle = AIASC->MakeOutgoingSpec(DamageGEClass,1.f,AIEffectContext);
	
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

		if (AISpecHandle.IsValid())
		{
			AIASC->ApplyGameplayEffectSpecToTarget(*AISpecHandle.Data.Get(),PlayerASC);
		}
	}

	Destroy();
}
