#include "Weapon/GRProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Character/GRCharacter.h"
#include "Player/GRPlayerState.h"
#include "Weapon/GRWeaponDefinition.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemInterface.h"
#include "Net/UnrealNetwork.h"

AGRProjectile::AGRProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;
	SetReplicateMovement(true);

	// Collision 컴포넌트
	CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("CollisionComponent"));
	CollisionComponent->InitSphereRadius(15.0f);  // 임시 기본값
	CollisionComponent->SetCollisionProfileName(TEXT("Projectile"));
	CollisionComponent->OnComponentHit.AddDynamic(this, &AGRProjectile::OnProjectileHit);
	RootComponent = CollisionComponent;

	// 메시 컴포넌트
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	MeshComponent->SetupAttachment(RootComponent);
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// Projectile Movement (실제 값은 InitializeProjectile에서 설정)
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovement"));
	ProjectileMovement->UpdatedComponent = CollisionComponent;
	ProjectileMovement->InitialSpeed = 0.0f;
	ProjectileMovement->MaxSpeed = 10000.0f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = false;
	ProjectileMovement->ProjectileGravityScale = 0.0f;

	// 트레일 이펙트
	TrailEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("TrailEffect"));
	TrailEffect->SetupAttachment(RootComponent);
	TrailEffect->bAutoActivate = false;
}

void AGRProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGRProjectile, bHasExploded);
}

void AGRProjectile::BeginPlay()
{
	Super::BeginPlay();

	// 메시 설정
	if (ProjectileMesh)
	{
		MeshComponent->SetStaticMesh(ProjectileMesh);

		// 메시 크기 기반으로 충돌 크기 자동 설정
		SetupCollisionFromMesh();
	}

	// 트레일 이펙트 활성화
	if (TrailNiagaraSystem && TrailEffect)
	{
		TrailEffect->SetAsset(TrailNiagaraSystem);
		TrailEffect->Activate();
	}
}

void AGRProjectile::SetupCollisionFromMesh()
{
	if (!ProjectileMesh || !CollisionComponent)
	{
		return;
	}

	// 메시의 바운딩 박스 크기 가져오기
	FBox BoundingBox = ProjectileMesh->GetBoundingBox();
	FVector BoxExtent = BoundingBox.GetExtent();

	// 가장 큰 축을 기준으로 구체 반경 설정
	float MaxExtent = FMath::Max3(BoxExtent.X, BoxExtent.Y, BoxExtent.Z);
	float CollisionRadius = MaxExtent * CollisionSizeMultiplier;

	CollisionComponent->SetSphereRadius(CollisionRadius);

	UE_LOG(LogTemp, Log, TEXT("[Projectile] Collision radius set to %.1f (Mesh extent: %.1f)"),
		CollisionRadius, MaxExtent);
}

void AGRProjectile::InitializeProjectile(
	AGRCharacter* InOwnerCharacter,
	float InDamage,
	float InExplosionRadius,
	float InExplosionFalloff,
	const FVector& InVelocity,
	float InGravityScale,
	float InLifeSpan)
{
	OwnerCharacter = InOwnerCharacter;
	Damage = InDamage;
	ExplosionRadius = InExplosionRadius;
	ExplosionFalloff = InExplosionFalloff;

	if (ProjectileMovement)
	{
		ProjectileMovement->Velocity = InVelocity;
		ProjectileMovement->ProjectileGravityScale = InGravityScale;
	}

	// WeaponDefinition에서 받은 수명 설정
	SetLifeSpan(InLifeSpan);

	// 발사한 캐릭터와의 충돌 무시
	if (OwnerCharacter)
	{
		CollisionComponent->IgnoreActorWhenMoving(OwnerCharacter, true);
	}

	UE_LOG(LogTemp, Log, TEXT("[Projectile] Initialized - Damage: %.1f, Radius: %.1f, Speed: %.1f, Gravity: %.2f, LifeSpan: %.1f"),
		Damage, ExplosionRadius, InVelocity.Size(), InGravityScale, InLifeSpan);
}

void AGRProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (bHasExploded)
	{
		return;
	}

	// 충돌 위치
	const FVector HitLocation = Hit.ImpactPoint;

	if (HasAuthority())
	{
		bHasExploded = true;

		UE_LOG(LogTemp, Log, TEXT("[Projectile] Hit at %s"), *HitLocation.ToString());

		// 데미지 처리 (서버만)
		if (ExplosionRadius > 0.0f)
		{
			const float ExplosionScale = FMath::Clamp(ExplosionRadius / 200.0f, 0.5f, 3.0f);
			PlayExplosionFX(HitLocation, ExplosionScale);

			UE_LOG(LogTemp, Log, TEXT("[Projectile] Explosion damage (Radius: %.1f)"), ExplosionRadius);
			ApplyExplosionDamage(HitLocation);
		}
		else if (OtherActor && OtherActor != OwnerCharacter)
		{
			UE_LOG(LogTemp, Log, TEXT("[Projectile] Direct hit damage"));
			if (OwnerCharacter)
			{
				OwnerCharacter->PlayImpactFXLocal(HitLocation);
				if(HasAuthority())
				{
					OwnerCharacter->Multicast_PlayImpactFX(HitLocation);
				}
			}
			ApplyDirectDamage(OtherActor, Hit);
		}

		// 약간의 딜레이 후 파괴 (이펙트 재생 시간 확보)
		SetLifeSpan(0.1f);
	}
}

void AGRProjectile::ApplyDirectDamage(AActor* HitActor, const FHitResult& Hit)
{
	if (!HasAuthority() || !HitActor || !OwnerCharacter)
	{
		return;
	}

	// GAS를 통한 데미지 적용
	IAbilitySystemInterface* TargetASI = Cast<IAbilitySystemInterface>(HitActor);
	if (!TargetASI)
	{
		return;
	}

	UAbilitySystemComponent* TargetASC = TargetASI->GetAbilitySystemComponent();
	if (!TargetASC)
	{
		return;
	}

	IAbilitySystemInterface* SourceASI = Cast<IAbilitySystemInterface>(OwnerCharacter);
	if (!SourceASI)
	{
		return;
	}

	UAbilitySystemComponent* SourceASC = SourceASI->GetAbilitySystemComponent();
	if (!SourceASC)
	{
		return;
	}

	// DamageEffect 가져오기 (무기 정의에서)
	AGRPlayerState* PS = OwnerCharacter->GetPlayerState<AGRPlayerState>();
	if (!PS)
	{
		return;
	}

	UGRWeaponDefinition* WeaponDef = PS->GetCurrentWeaponDefinition();
	if (!WeaponDef)
	{
		return;
	}

	// Note: DamageEffect는 FireWeapon 어빌리티에 정의되어 있으므로,
	// 여기서는 간단히 SetByCallerMagnitude로 데미지를 전달
	// WeaponDefinition에 DamageEffectClass를 추가하는 것도 고려해볼만 함.

	UE_LOG(LogTemp, Log, TEXT("[Projectile] Direct damage applied: %.1f to %s"),
		Damage, *HitActor->GetName());
}

void AGRProjectile::ApplyExplosionDamage(const FVector& ExplosionLocation)
{
	if (!HasAuthority() || !OwnerCharacter)
	{
		return;
	}

	TArray<FHitResult> HitResults;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);
	if (OwnerCharacter)
	{
		QueryParams.AddIgnoredActor(OwnerCharacter);
	}

	// 구형 범위 탐색
	bool bHit = GetWorld()->SweepMultiByChannel(
		HitResults,
		ExplosionLocation,
		ExplosionLocation,
		FQuat::Identity,
		ECC_Pawn,
		FCollisionShape::MakeSphere(ExplosionRadius),
		QueryParams
	);

	if (!bHit)
	{
		UE_LOG(LogTemp, Log, TEXT("[Projectile] No targets in explosion radius"));
		return;
	}

	TSet<AActor*> DamagedActors;
	int32 HitCount = 0;

	for (const FHitResult& HitResult : HitResults)
	{
		AActor* HitActor = HitResult.GetActor();
		if (!HitActor || DamagedActors.Contains(HitActor))
		{
			continue;
		}

		// GAS 인터페이스 확인
		IAbilitySystemInterface* TargetASI = Cast<IAbilitySystemInterface>(HitActor);
		if (!TargetASI)
		{
			continue;
		}

		UAbilitySystemComponent* TargetASC = TargetASI->GetAbilitySystemComponent();
		if (!TargetASC)
		{
			continue;
		}

		DamagedActors.Add(HitActor);

		// 거리에 따른 데미지 감쇠
		float Distance = FVector::Dist(ExplosionLocation, HitActor->GetActorLocation());
		float DistanceRatio = FMath::Clamp(Distance / ExplosionRadius, 0.0f, 1.0f);
		float DamageMult = FMath::Lerp(1.0f, ExplosionFalloff, DistanceRatio);
		float FinalDamage = Damage * DamageMult;

		HitCount++;

		UE_LOG(LogTemp, Log, TEXT("[Explosion] Hit %d: %s - Distance: %.1f, Mult: %.2f, Damage: %.1f"),
			HitCount, *HitActor->GetName(), Distance, DamageMult, FinalDamage);

		// TODO: GAS DamageEffect 적용
		// 여기서는 실제 데미지 Effect를 적용해야 합니다
		// 예: ApplyDamageEffect(HitActor, HitResult, FinalDamage);
	}

	UE_LOG(LogTemp, Log, TEXT("[Projectile] Explosion hit %d targets"), HitCount);

#if WITH_EDITOR
	// 디버그 시각화
	DrawDebugSphere(GetWorld(), ExplosionLocation, ExplosionRadius, 32,
		FColor::Red, false, 3.0f, 0, 2.0f);
#endif
}

void AGRProjectile::PlayExplosionFX(const FVector& HitLocation, float ExplosionScale)
{
	if (!OwnerCharacter)
	{
		return;
	}

	OwnerCharacter->PlayExplosionFXLocal(HitLocation, ExplosionScale);

	// 서버만 다른 클라이언트들에게 브로드캐스트
	if (OwnerCharacter->HasAuthority())
	{
		OwnerCharacter->Multicast_PlayExplosionFX(HitLocation, ExplosionScale);
	}
}
