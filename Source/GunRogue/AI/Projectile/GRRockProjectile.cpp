// Fill out your copyright notice in the Description page of Project Settings.


#include "GRRockProjectile.h"
#include "AI/Character/GRLuwoAICharacter.h"
#include "Components/SphereComponent.h"
#include "Engine/OverlapResult.h"
#include "Character/GRCharacter.h"
#include "GameFramework/DamageType.h"
#include "AbilitySystemComponent.h"
#include "GameplayEffect.h"
#include "AbilitySystemInterface.h"

AGRRockProjectile::AGRRockProjectile()
	:DamageGEClass(nullptr)
{
	PrimaryActorTick.bCanEverTick = false;

	bReplicates = true;
	SetReplicateMovement(true);
	
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	CollisionComponent->BodyInstance.SetCollisionProfileName(TEXT("AIProjectile"));
	CollisionComponent->SetSimulatePhysics(false);
	CollisionComponent->SetNotifyRigidBodyCollision(true);
	CollisionComponent->SetIsReplicated(true);
	RootComponent = CollisionComponent;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(RootComponent);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AGRRockProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if (CollisionComponent)
		{
			CollisionComponent->OnComponentHit.AddDynamic(this, &AGRRockProjectile::OnHit);
		}
	}
}

void AGRRockProjectile::Throw(const FVector& LaunchVelocity)
{
	if(!HasAuthority())
	{
		return;
	}
	
	if (CollisionComponent)
	{
		CollisionComponent->SetSimulatePhysics(true);
		CollisionComponent->AddImpulse(LaunchVelocity, NAME_None, true);
	}
}

void AGRRockProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
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
	
	FVector Origin = Hit.ImpactPoint;
	const float Radius = 500.f;
	TArray<FOverlapResult> Overlaps;

	FCollisionShape SphereShape = FCollisionShape::MakeSphere(Radius);
	FCollisionQueryParams QueryParams;
	
	ECollisionChannel TraceChannel = ECC_Pawn;

	bool bOverlap = GetWorld()->OverlapMultiByChannel(
		Overlaps,
		Origin,               
		FQuat::Identity,      
		TraceChannel,
		SphereShape,
		QueryParams
	);
	
#if WITH_EDITOR
	DrawDebugSphere(GetWorld(), Origin, Radius, 16, FColor::Yellow, false, 1.0f);
#endif
	
	if (!bOverlap)
	{
		return;
	}

	UAbilitySystemComponent* BossASC = GetInstigator()->FindComponentByClass<UAbilitySystemComponent>();
	if (!BossASC)
	{
		return;
	}

	if (!DamageGEClass)
	{
		return;
	}
	
	FGameplayEffectContextHandle BossEffectContext = BossASC->MakeEffectContext();
	BossEffectContext.AddInstigator(GetInstigator(), this); 
		
	FGameplayEffectSpecHandle BossSpecHandle = BossASC->MakeOutgoingSpec(DamageGEClass,1.f,BossEffectContext);
	
	for (const FOverlapResult& Result : Overlaps)
	{
		AActor* Other = Result.GetActor();
		if (!Other)
		{
			continue;
		}
		
		AGRCharacter* PlayerChar=Cast<AGRCharacter>(Other);
		if (!PlayerChar)
		{
			continue;
		}
		
		IAbilitySystemInterface* PlayerASI = Cast<IAbilitySystemInterface>(PlayerChar);
		if (!PlayerASI)
		{
			continue;
		}
		
		UAbilitySystemComponent* PlayerASC = PlayerASI->GetAbilitySystemComponent();
		if (!PlayerASC)
		{
			continue;
		}

#if WITH_EDITOR
		FVector PlayerLoc = PlayerChar->GetActorLocation();
		if (GetWorld())
		{
			DrawDebugSphere(GetWorld(),PlayerLoc,20.f,12,FColor::Red,false,1.0f);
		}
#endif

		if (BossSpecHandle.IsValid())
		{
			BossASC->ApplyGameplayEffectSpecToTarget(*BossSpecHandle.Data.Get(),PlayerASC);
		}
	}
	
	Destroy();
}



