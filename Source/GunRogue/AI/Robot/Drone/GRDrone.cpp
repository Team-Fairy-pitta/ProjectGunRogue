// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Robot/Drone/GRDrone.h"
#include "AI/Robot/Drone/GRDroneAIController.h"
#include "AI/Robot/Drone/GRDroneManagerComponent.h"
#include "AI/Character/GRAICharacter.h"
#include "AI/Projectile/GRDroneProjectile.h"
#include "Player/GRPlayerState.h"
#include "Character/GRCharacter.h"
#include "AI/Robot/AT/GRSkillAttributeSet_Robot.h"
#include "Engine/OverlapResult.h"
#include "Net/UnrealNetwork.h"

AGRDrone::AGRDrone()
	:FollowTarget(nullptr)
	,CircleRadius(80.0f)
	,CircleCenter(FVector(-30.0f,0.0f,40.0f))
	,MoveInterpSpeed(6.0f)
	,StopDistance(10.0f)
	,RotationInterpSpeed(6.0f)
{
	PrimaryActorTick.bCanEverTick = true;
	
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(10.0f);
	CollisionComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	CollisionComp->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	RootComponent = CollisionComp;
	
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	DetectSphere = CreateDefaultSubobject<USphereComponent>(TEXT("DetectSphere"));
	DetectSphere->SetupAttachment(RootComponent);
	DetectSphere->SetSphereRadius(1500.f);
	DetectSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DetectSphere->SetCollisionObjectType(ECC_WorldDynamic);
	DetectSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	DetectSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AGRDroneAIController::StaticClass();
	
	bReplicates = true;
}

void AGRDrone::SetTargetOffset(AActor* Player,int32 Index,int TotalNum)
{
	FollowTarget = Player;
	
	float AngleStep = 180.f / (TotalNum + 1);
	float AngleDeg  = -90.f + AngleStep * (Index + 1);
	float AngleRad  = FMath::DegreesToRadians(AngleDeg);
	
	TargetOffset = CircleCenter +
	FVector(
		0.f,
		FMath::Sin(AngleRad) * CircleRadius,
		FMath::Cos(AngleRad) * CircleRadius
	);
}

void AGRDrone::StartMainSkillAttack(const FVector& InDir)
{
	MainSkillDir = InDir.GetSafeNormal();
	DroneState = EDroneState::MainSkillAttack;
	bIsMainSkillActive = true;
}

void AGRDrone::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		DetectSphere->OnComponentBeginOverlap.AddDynamic(this, &AGRDrone::OnDetectBegin);
		DetectSphere->OnComponentEndOverlap.AddDynamic(this, &AGRDrone::OnDetectEnd);
		
		float Duration = GetDurationFromInstigator();

		 GetWorld()->GetTimerManager().SetTimer(
                ExplodeTimerHandle,
                this,
                &AGRDrone::Explode,
                Duration,
                false
            );
	}
}

void AGRDrone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		UpdateDroneStateMachine();
	}

	ApplyMovement();
}

void AGRDrone::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (HasAuthority())
	{
		AActor* GRCharacter = GetInstigator();
		if (GRCharacter)
		{
			UGRDroneManagerComponent* Manager = GRCharacter->FindComponentByClass<UGRDroneManagerComponent>();
			if (Manager)
			{
				Manager->RemoveDrone(this);
			}
		}
	}
}

void AGRDrone::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGRDrone, DesiredLocation);
	DOREPLIFETIME(AGRDrone, DesiredRotation);
}

void AGRDrone::OnDetectBegin(UPrimitiveComponent* Overlapped, AActor* OtherActor, UPrimitiveComponent* OtherComp,
                             int32 BodyIndex, bool bFromSweep, const FHitResult& Hit)
{
	if (!HasAuthority())
	{
		return;
	}
	
	if (OtherActor->IsA(AGRAICharacter::StaticClass()))
	{
		DetectedEnemies.AddUnique(OtherActor);
	}
}

void AGRDrone::OnDetectEnd(UPrimitiveComponent* Overlapped, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 BodyIndex)
{
	if (!HasAuthority())
	{
		return;
	}

	if (OtherActor->IsA(AGRAICharacter::StaticClass()))
	{
		DetectedEnemies.Remove(OtherActor);
	}
}

void AGRDrone::FollowPlayer()
{
	if (!IsValid(FollowTarget))
	{
		return;
	}
	
	const FTransform PlayerTransform = FollowTarget->GetActorTransform();
	DesiredLocation = PlayerTransform.TransformPosition(TargetOffset);
}

void AGRDrone::FaceForward()
{
	if (!IsValid(FollowTarget))
	{
		return;
	}
	
	DesiredRotation  = FollowTarget->GetActorRotation();
	DesiredRotation.Pitch = 0.f;
	DesiredRotation.Roll  = 0.f;
}

void AGRDrone::SetCloseTarget()
{
	AActor* Target = nullptr;
	
	float BestDistSq = FLT_MAX;

	for (auto& WeakEnemy : DetectedEnemies)
	{
		if (!WeakEnemy.IsValid())
		{
			continue;
		}
		
		float DistSq = FVector::DistSquared(
			GetActorLocation(),
			WeakEnemy->GetActorLocation());

		if (DistSq < BestDistSq && CanFireAtTarget(WeakEnemy.Get()))
		{
			BestDistSq = DistSq;
			Target = WeakEnemy.Get();
		}
	}

	AITarget = Target;
}

bool AGRDrone::CanFireAtTarget(AActor* Target)
{
	if (!Target)
	{
		return false;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}
	
	const FVector Start = Mesh->GetSocketLocation(MuzzleSocketName);
	const FVector End   = Target->GetActorLocation();

	FHitResult Hit;
	FCollisionQueryParams Params;
	Params.bTraceComplex = false;
	Params.AddIgnoredActor(this);   
	Params.AddIgnoredActor(GetInstigator());   

	bool bHit = World->LineTraceSingleByChannel(
		Hit,
		Start,
		End,
		ECC_EngineTraceChannel3,
		Params
	);

	AActor* HitActor=Hit.GetActor();
	if (HitActor)
	{
	   if (HitActor->IsA(AGRDroneProjectile::StaticClass()))
	   {
		   return true;
	   }
	}
	
// #if ENABLE_DRAW_DEBUG
// 	bool bCanDetect = false;
// 	
// 	if (bHit && Hit.GetActor() == Target)
// 	{
// 		bCanDetect = true;
// 		
// 		DrawDebugLine(
// 			World,
// 			Start,
// 			End,
// 			bCanDetect ? FColor::Red : FColor::Green,
// 			false,
// 			0.1f,
// 			0,
// 			1.5f
// 		);
// 	}
// 	
// #endif
	
	return bHit && Hit.GetActor() == Target;
}

void AGRDrone::UpdateState()
{
	if (DroneState == EDroneState::MainSkillAttack)
	{
		return;
	}
	
	if (AITarget)
	{
		DroneState = EDroneState::Fire;
	}
	else
	{
		DroneState = EDroneState::Idle;
	}
}

void AGRDrone::FaceTarget()
{
	if (!AITarget)
	{
		return;
	}
	
	DesiredRotation = (AITarget->GetActorLocation() - DesiredLocation).Rotation();
}

void AGRDrone::Fire()
{
	const float DeltaTime = GetWorld()->GetDeltaSeconds();
	
	FireCooldown -= DeltaTime;

	if (FireCooldown > 0.f)
	{
		return;
	}
	
	if (!CanFireAtTarget(AITarget))
	{
		return;
	}
	
	FireCooldown += GetFireIntervalFromInstigator();

	FVector MuzzleLocation =
		Mesh->DoesSocketExist(MuzzleSocketName)
		? Mesh->GetSocketLocation(MuzzleSocketName)
		: GetActorLocation();

	FVector FireDir = (AITarget->GetActorLocation() - MuzzleLocation).GetSafeNormal();

	FRotator FireRot = FireDir.Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	SpawnParams.Instigator = GetInstigator();
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	AActor* Bullet = GetWorld()->SpawnActor<AActor>(
		BulletClass,
		MuzzleLocation,
		FireRot,
		SpawnParams
	);
}

void AGRDrone::DashToDirection()
{
	const float DeltaTime = GetWorld()->GetDeltaSeconds();

	DesiredLocation = GetActorLocation() + MainSkillDir * MainSkillSpeed * DeltaTime;
}

void AGRDrone::Explode()
{
	if (!HasAuthority())
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

	bIsMainSkillActive = false;
	
	FVector Origin = GetActorLocation();
	TArray<FOverlapResult> Overlaps;

	float ExplodeDistance = GetExplodeDistanceFromInstigator();

	FCollisionShape SphereShape = FCollisionShape::MakeSphere(ExplodeDistance);
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
	DrawDebugSphere(GetWorld(), Origin, ExplodeDistance, 16, FColor::Yellow, false, 1.0f);
#endif
	
	if (!bOverlap)
	{
		return;
	}

	if (!ExplodeDamageGEClass)
	{
		return;
	}
	
	FGameplayEffectContextHandle PlayerEffectContext = PlayerASC->MakeEffectContext();
	PlayerEffectContext.AddSourceObject(this);
		
	FGameplayEffectSpecHandle PlayerSpecHandle = PlayerASC->MakeOutgoingSpec(ExplodeDamageGEClass,1.f,PlayerEffectContext);
	
	float Damage = GetExplodeDamageFromInstigator();
	PlayerSpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Ability.Damage.DroneExplode")), Damage);

	for (const FOverlapResult& Result : Overlaps)
	{
		AActor* Other = Result.GetActor();
		if (!Other)
		{
			continue;
		}
		
		AGRAICharacter* AIChar=Cast<AGRAICharacter>(Other);
		if (!AIChar)
		{
			continue;
		}
		
		IAbilitySystemInterface* AIASI = Cast<IAbilitySystemInterface>(AIChar);
		if (!AIASI)
		{
			continue;
		}
		
		UAbilitySystemComponent* AIASC = AIASI->GetAbilitySystemComponent();
		if (!AIASC)
		{
			continue;
		}

#if WITH_EDITOR
		FVector AILoc = AIChar->GetActorLocation();
		if (GetWorld())
		{
			DrawDebugSphere(GetWorld(),AILoc,20.f,12,FColor::Red,false,1.0f);
		}
#endif

		if (PlayerSpecHandle.IsValid())
		{
			PlayerASC->ApplyGameplayEffectSpecToTarget(*PlayerSpecHandle.Data.Get(),AIASC);
		}
	}
	
	FGameplayTag ImpactCueTag = FGameplayTag::RequestGameplayTag("GameplayCue.AI.Drone.Explode");
	FGameplayCueParameters Params;
	Params.Location = GetActorLocation();
	PlayerASC->ExecuteGameplayCue(ImpactCueTag,Params);
	
	Destroy();
}

void AGRDrone::ApplyMovement()
{
	const float DeltaTime = GetWorld()->GetDeltaSeconds();

	const bool bDash = (DroneState == EDroneState::MainSkillAttack);
	
	if (bDash)
	{
		FHitResult Hit;
		SetActorLocation(DesiredLocation, true, &Hit);

		if (HasAuthority() && Hit.bBlockingHit)
		{
			Explode();
		}

		SetActorRotation(DesiredRotation);
	}
	else
	{
		SetActorLocation(
			FMath::VInterpTo(
				GetActorLocation(),
				DesiredLocation,
				DeltaTime,
				MoveInterpSpeed
			)
		);

		SetActorRotation(
			FMath::RInterpTo(
				GetActorRotation(),
				DesiredRotation,
				DeltaTime,
				RotationInterpSpeed
			)
		);
	}
}

void AGRDrone::UpdateDroneStateMachine()
{
	const float DeltaTime = GetWorld()->GetDeltaSeconds();
	
	TargetUpdateTimer -= DeltaTime;
	if (TargetUpdateTimer <= 0.f)
	{
		SetCloseTarget();
		UpdateState();
		TargetUpdateTimer = TargetUpdateInterval;
	}
		
	switch (DroneState)
	{
	case EDroneState::Idle:
		FollowPlayer();
		FaceForward();
		break;

	case EDroneState::Fire:
		FollowPlayer();
		FaceTarget();
		Fire();
		break;

	case EDroneState::MainSkillAttack:
		DashToDirection();
		break;
	}
}

float AGRDrone::GetDurationFromInstigator() const
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

	return SkillAttributeSet->GetDuration();
}

float AGRDrone::GetFireIntervalFromInstigator() const
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

	return SkillAttributeSet->GetAttackSpeed() * FireInterval;
}

float AGRDrone::GetExplodeDistanceFromInstigator() const
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

	return SkillAttributeSet->GetSelfDestructRadius();
}

float AGRDrone::GetExplodeDamageFromInstigator() const
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

	return SkillAttributeSet->GetSelfDestructDamage();
}


