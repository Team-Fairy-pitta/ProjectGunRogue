#include "TestAttributes/TestProjectile.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "AbilitySystemInterface.h"
#include "GunRogue/AbilitySystem/GRAbilitySystemComponent.h"

// Sets default values
ATestProjectile::ATestProjectile()
{
	PrimaryActorTick.bCanEverTick = false;

	// Collision
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(15.0f);
	CollisionComp->SetCollisionProfileName(TEXT("Projectile"));
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComp->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	CollisionComp->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	CollisionComp->SetCollisionResponseToChannel(ECC_Pawn, ECollisionResponse::ECR_Block);
	CollisionComp->SetCollisionResponseToChannel(ECC_WorldStatic, ECollisionResponse::ECR_Block);
	CollisionComp->SetNotifyRigidBodyCollision(true);
	RootComponent = CollisionComp;

	// Mesh
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetupAttachment(CollisionComp);
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 1000.0f;
	ProjectileMovement->MaxSpeed = 1000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;

}

// Called when the game starts or when spawned
void ATestProjectile::BeginPlay()
{
	Super::BeginPlay();

	CollisionComp->OnComponentHit.AddDynamic(this, &ATestProjectile::OnHit);
}

void ATestProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!OtherActor || OtherActor == GetOwner())
	{
		return;
	}

	// AbilitySystemComponent 찾기
	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(OtherActor);
	if (ASI)
	{
		UAbilitySystemComponent* TargetASC = ASI->GetAbilitySystemComponent();
		if (TargetASC && EffectToApply)
		{
			// GameplayEffect 적용
			FGameplayEffectContextHandle EffectContext = TargetASC->MakeEffectContext();
			EffectContext.AddSourceObject(this);

			FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(
				EffectToApply, 1.0f, EffectContext);

			if (SpecHandle.IsValid())
			{
				TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

				UE_LOG(LogTemp, Log, TEXT("[Projectile] Applied %s to %s"),
					*EffectToApply->GetName(), *OtherActor->GetName());
			}
		}
	}

	Destroy();
}

