#include "Weapon/GRProjectile.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Character/GRCharacter.h"
#include "AI/Character/GRAICharacter.h"
#include "Player/GRPlayerState.h"
#include "Weapon/GRWeaponDefinition.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemInterface.h"
#include "GameplayEffect.h"
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
	DOREPLIFETIME(AGRProjectile, DamageEffectClass);
	DOREPLIFETIME(AGRProjectile, ExplosionEffectNiagara);
	DOREPLIFETIME(AGRProjectile, ExplosionEffectCascade);
	DOREPLIFETIME(AGRProjectile, ExplosionSound);
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
	float InLifeSpan,
	TSubclassOf<UGameplayEffect> InDamageEffect,
	UNiagaraSystem* InExplosionEffectNiagara,
	UParticleSystem* InExplosionEffectCascade,
	USoundBase* InExplosionSound)
{
	OwnerCharacter = InOwnerCharacter;
	Damage = InDamage;
	ExplosionRadius = InExplosionRadius;
	ExplosionFalloff = InExplosionFalloff;
	DamageEffectClass = InDamageEffect;
	ExplosionEffectNiagara = InExplosionEffectNiagara;
	ExplosionEffectCascade = InExplosionEffectCascade;
	ExplosionSound = InExplosionSound;

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

	// 같은 Owner가 쏜 다른 투사체들과 충돌 무시
	if (CollisionComponent && OwnerCharacter)
	{
		TArray<AActor*> FoundProjectiles;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGRProjectile::StaticClass(), FoundProjectiles);

		for (AActor* OtherActor : FoundProjectiles)
		{
			AGRProjectile* OtherProjectile = Cast<AGRProjectile>(OtherActor);
			if (OtherProjectile && OtherProjectile != this)
			{
				// 같은 Owner가 쏜 투사체끼리만 무시
				if (OtherProjectile->OwnerCharacter == OwnerCharacter)
				{
					CollisionComponent->IgnoreActorWhenMoving(OtherProjectile, true);

					// 상호 무시 설정
					if (OtherProjectile->CollisionComponent)
					{
						OtherProjectile->CollisionComponent->IgnoreActorWhenMoving(this, true);
					}
				}
			}
		}
	}

	// 모든 GRProjectile 투사체와 충돌 무시 하려면 아래 코드 사용
	/*
	// 모든 투사체와 충돌 무시
	if (CollisionComponent)
	{
		TArray<AActor*> AllProjectiles;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), AGRProjectile::StaticClass(), AllProjectiles);

		for (AActor* OtherActor : AllProjectiles)
		{
			if (OtherActor != this)
			{
				CollisionComponent->IgnoreActorWhenMoving(OtherActor, true);
				
				// ✅ 양방향 무시 (상대도 나를 무시하도록)
				if (AGRProjectile* OtherProjectile = Cast<AGRProjectile>(OtherActor))
				{
					if (OtherProjectile->CollisionComponent)
					{
						OtherProjectile->CollisionComponent->IgnoreActorWhenMoving(this, true);
					}
				}
			}
		}
	}
	*/

	UE_LOG(LogTemp, Log, TEXT("[Projectile] Initialized - Damage: %.1f, Radius: %.1f, Speed: %.1f, Gravity: %.2f, LifeSpan: %.1f"),
		Damage, ExplosionRadius, InVelocity.Size(), InGravityScale, InLifeSpan);
}

void AGRProjectile::OnProjectileHit(UPrimitiveComponent* HitComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (!HasAuthority())
	{
		return;
	}

	if (bHasExploded)
	{
		return;
	}

	if (OtherActor && OtherActor->IsA<AGRProjectile>())
	{
		return;
	}

	if (OtherActor == OwnerCharacter)
	{
		return;
	}


	// 충돌 위치
	const FVector HitLocation = Hit.ImpactPoint;

	bHasExploded = true;

	float ExplosionScale = 0.5f;
	if (ExplosionRadius > 0.0f)
	{
		ExplosionScale = FMath::Clamp(ExplosionRadius / 200.0f, 0.5f, 3.0f);
		ApplyExplosionDamage(HitLocation);
	}
	else if (OtherActor && OtherActor != OwnerCharacter)
	{
		ApplyDirectDamage(OtherActor, Hit);
	}

	// Multicast로 모든 클라이언트에서 이펙트 재생
	Multicast_PlayExplosionFX(HitLocation, ExplosionScale);

	// 약간의 딜레이 후 파괴
	SetLifeSpan(0.1f);
}

void AGRProjectile::ApplyDirectDamage(AActor* HitActor, const FHitResult& Hit)
{
	if (!HasAuthority() || !HitActor || !OwnerCharacter)
	{
		return;
	}

	if (!DamageEffectClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Projectile] No DamageEffectClass set"));
		return;
	}

	if (!HitActor->IsA(AGRAICharacter::StaticClass()))
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

	// GameplayEffect 생성 및 적용
	FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
	EffectContext.AddSourceObject(OwnerCharacter);
	EffectContext.AddHitResult(Hit);

	FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(
		DamageEffectClass, 1.0f, EffectContext);

	if (!SpecHandle.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("[Projectile] Invalid SpecHandle"));
		return;
	}

	// SetByCaller로 데미지 전달
	SpecHandle.Data->SetSetByCallerMagnitude(
		FGameplayTag::RequestGameplayTag(FName("Attribute.Data.Damage")),
		Damage);

	// 데미지 적용
	SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);

	UE_LOG(LogTemp, Log, TEXT("[Projectile] Direct damage applied: %.1f to %s"),
		Damage, *HitActor->GetName());

#if WITH_EDITOR
	if (GEngine)
	{
		const FString Msg = FString::Printf(TEXT("Projectile Hit: %.1f damage"), Damage);
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Red, Msg);
	}
#endif
}

void AGRProjectile::ApplyExplosionDamage(const FVector& ExplosionLocation)
{
	if (!HasAuthority() || !OwnerCharacter)
	{
		return;
	}

	if (!DamageEffectClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Projectile] No DamageEffectClass for explosion"));
		return;
	}

	// Source ASC 가져오기
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

		// AI 캐릭터만 데미지 적용
		if (!HitActor->IsA(AGRAICharacter::StaticClass()))
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

		// GameplayEffect 생성 및 적용
		FGameplayEffectContextHandle EffectContext = SourceASC->MakeEffectContext();
		EffectContext.AddSourceObject(OwnerCharacter);
		EffectContext.AddHitResult(HitResult);

		FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(
			DamageEffectClass, 1.0f, EffectContext);

		if (SpecHandle.IsValid())
		{
			// SetByCaller로 감쇠된 데미지 전달
			SpecHandle.Data->SetSetByCallerMagnitude(
				FGameplayTag::RequestGameplayTag(FName("Attribute.Data.Damage")),
				FinalDamage);

			SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);

			HitCount++;

			UE_LOG(LogTemp, Log, TEXT("[Explosion] Hit %d: %s - Distance: %.1f, Mult: %.2f, Damage: %.1f"),
				HitCount, *HitActor->GetName(), Distance, DamageMult, FinalDamage);
		}
	}

	UE_LOG(LogTemp, Log, TEXT("[Projectile] Explosion hit %d targets"), HitCount);

#if WITH_EDITOR
	// 디버그 시각화
	DrawDebugSphere(GetWorld(), ExplosionLocation, ExplosionRadius, 32,
		FColor::Red, false, 3.0f, 0, 2.0f);

	if (GEngine)
	{
		const FString Msg = FString::Printf(TEXT("Explosion: %d targets hit"), HitCount);
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, Msg);
	}
#endif
}

void AGRProjectile::Multicast_PlayExplosionFX_Implementation(const FVector& HitLocation, float ExplosionScale)
{
	PlayExplosionFX(HitLocation, ExplosionScale);
}

void AGRProjectile::PlayExplosionFX(const FVector& HitLocation, float ExplosionScale)
{
	// 나이아가라 우선
	if (ExplosionEffectNiagara)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			ExplosionEffectNiagara,
			HitLocation,
			FRotator::ZeroRotator,
			FVector(ExplosionScale),
			true,
			true,
			ENCPoolMethod::AutoRelease
		);
	}
	// 캐스케이드 대체
	else if (ExplosionEffectCascade)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			ExplosionEffectCascade,
			HitLocation,
			FRotator::ZeroRotator,
			FVector(ExplosionScale),
			true,
			EPSCPoolMethod::AutoRelease
		);
	}

	// 사운드 재생
	if (ExplosionSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			ExplosionSound,
			HitLocation
		);
	}

	UE_LOG(LogTemp, Log, TEXT("[Projectile] Explosion FX played at %s"), *HitLocation.ToString());
}

