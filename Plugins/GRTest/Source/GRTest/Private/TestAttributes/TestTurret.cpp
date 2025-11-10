#include "TestAttributes/TestTurret.h"
#include "TestAttributes/TestProjectile.h"

ATestTurret::ATestTurret()
{
	PrimaryActorTick.bCanEverTick = true;

	Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(Root);

	BaseMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BaseMesh"));
	BaseMesh->SetupAttachment(Root);
}

// Called when the game starts or when spawned
void ATestTurret::BeginPlay()
{
	Super::BeginPlay();

	TimeSinceLastFire = FireInterval; // 즉시 발사
}

// Called every frame
void ATestTurret::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TimeSinceLastFire += DeltaTime;
    
    if (TimeSinceLastFire >= FireInterval)
    {
        FireProjectile();
        TimeSinceLastFire = 0.0f;
    }

}

void ATestTurret::FireProjectile()
{
	if (!ProjectileClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Turret] ProjectileClass not set!"));
		return;
	}

	FVector SpawnLocation = GetActorLocation() + GetActorForwardVector() * 100.0f;
	FRotator SpawnRotation = GetActorRotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();

	ATestProjectile* Projectile = GetWorld()->SpawnActor<ATestProjectile>(
		ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams);

	if (Projectile)
	{
		// UE_LOG(LogTemp, Log, TEXT("[Turret] Fired projectile"));
	}
}

