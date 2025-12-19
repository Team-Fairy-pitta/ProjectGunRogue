// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Character/GRNormalAICharacter.h"
#include "AI/Controller/GRNormalAIController.h"
#include "AI/Data/GRAIWeaponData.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Abilities/GameplayAbility.h"
#include "Net/UnrealNetwork.h"
#include "Components/SkeletalMeshComponent.h"

AGRNormalAICharacter::AGRNormalAICharacter()
	:WeaponCategory(EGRAIWeaponCategory::None)
	,WeaponClass(nullptr)
	,WeaponActor(nullptr)
	,EquippedSocketName(FName("RightHandSocket"))
	,WeaponDataTable(nullptr)
	,AimPitch(0.0f)
	,AimYaw(0.0f)
	,AimPitchTarget(0.0f)
	,AimYawTarget(0.0f)
	,AimUpdateTimer(0.0f)
	,AimInterpSpeedDeg(180.0f)
	,AimUpdateInterval(0.1f)
{
	PrimaryActorTick.bCanEverTick = true; 
}

void AGRNormalAICharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SetWeaponData();
		SpawnAndAttachWeapon();
	}
}

void AGRNormalAICharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGRNormalAICharacter, AimPitchTarget);
	DOREPLIFETIME(AGRNormalAICharacter, AimYawTarget);
}

void AGRNormalAICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		Server_UpdateAimOffset(DeltaTime);
	}

	FRotator Current(AimPitch, AimYaw, 0.f);
	FRotator Target(AimPitchTarget, AimYawTarget, 0.f);

	FRotator NewRot = FMath::RInterpConstantTo(
		Current,
		Target,
		DeltaTime,
		AimInterpSpeedDeg
	);

	AimYaw   = NewRot.Yaw;
	AimPitch = NewRot.Pitch;
}

void AGRNormalAICharacter::SpawnAndAttachWeapon()
{
	if(WeaponCategory == EGRAIWeaponCategory::None)
	{
		return;
	}

	if (!WeaponClass)
	{
		return;
	}

	FVector SpawnLocation = GetMesh()->GetSocketLocation(EquippedSocketName);
	FRotator SpawnRot = GetActorRotation(); 
	FTransform SpawnTransform(SpawnRot, SpawnLocation);

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = this;
	WeaponActor = GetWorld()->SpawnActor<AActor>(WeaponClass, SpawnTransform, SpawnParams);
	if (!WeaponActor)
	{
		return;
	}
	
	WeaponActor->SetReplicates(true);
	WeaponActor->SetReplicateMovement(true);

	FAttachmentTransformRules AttachRules(
			EAttachmentRule::SnapToTarget,   // 위치
			EAttachmentRule::SnapToTarget,   // 회전
			EAttachmentRule::KeepWorld,      // 스케일
			true
		);

	WeaponActor->AttachToComponent(
		GetMesh(),
		AttachRules,
		EquippedSocketName
	);
}

void AGRNormalAICharacter::SetWeaponData()
{
	if (!WeaponDataTable)
	{
		return;
	}

	if (WeaponCategory==EGRAIWeaponCategory::None)
	{
		return;
	}

	AGRNormalAIController* NormalAIController = Cast<AGRNormalAIController>(GetController());
	if (!NormalAIController)
	{
		return;
	}

	UBlackboardComponent* BBComp = NormalAIController->GetBlackboardComponent();
	if (!BBComp)
	{
		return;
	}
	
	FString EnumString = UEnum::GetValueAsString(WeaponCategory);

	FString RightPart;
	EnumString.Split(TEXT("::"), nullptr, &RightPart);

	FName RowName = FName(*RightPart);

	const FString Context = TEXT("WeaponDataTable Lookup");
	FGRAIWeaponData* Data = WeaponDataTable->FindRow<FGRAIWeaponData>(RowName, Context);
	if (!Data)
	{
		return;
	}

	WeaponClass = Data->WeaponClass;
	EquippedSocketName = Data->EquipSocketName;

	BBComp->SetValueAsClass(AGRNormalAIController::FireWithWeaponAbilityKey, Data->AbilityToUse);
}

void AGRNormalAICharacter::Server_UpdateAimOffset(float DeltaTime)
{
	AimUpdateTimer += DeltaTime;
	if (AimUpdateTimer < AimUpdateInterval)
	{
		return;
	}

	AimUpdateTimer = 0.f;
	
	if (!WeaponActor)
	{
		return;
	}

	AGRNormalAIController* NormalAIController = Cast<AGRNormalAIController>(GetController());
	if (!NormalAIController)
	{
		return;
	}

	UBlackboardComponent* BB = NormalAIController->GetBlackboardComponent();
	if (!BB)
	{
		return;
	}

	AActor* TargetActor = Cast<AActor>(BB->GetValueAsObject(AGRNormalAIController::TargetPlayerKey));
	if (!TargetActor)
	{
		AimYawTarget   = 0.0f;
		AimPitchTarget = 0.0f;
		return;
	}

	FVector AILocation = GetActorLocation();
	FVector TargetLocation = TargetActor->GetActorLocation();
	FVector ToTarget = TargetLocation - AILocation;
	ToTarget = (TargetLocation - AILocation).GetSafeNormal();

	const FVector BaseForward = GetActorForwardVector();
	const FVector BaseRight   = GetActorRightVector();
	const FVector BaseUp      = GetActorUpVector();

	// Yaw 계산 (좌우)
	float YawRad = FMath::Atan2(
		FVector::DotProduct(ToTarget, BaseRight),
		FVector::DotProduct(ToTarget, BaseForward)
	);
	YawRad = FMath::RadiansToDegrees(YawRad);
	YawRad = FMath::Clamp(YawRad, -90.f, 90.f);
	
	// Pitch 계산 (상하)
	float PitchRad = FMath::Atan2(
		FVector::DotProduct(ToTarget, BaseUp),
		FVector::DotProduct(ToTarget, BaseForward)
	);
	PitchRad = FMath::RadiansToDegrees(PitchRad);
	PitchRad = FMath::Clamp(PitchRad, -45.f, 45.f);

	if (FMath::Abs(YawRad - AimYawTarget) > 1.f ||
		FMath::Abs(PitchRad - AimPitchTarget) > 1.f)
	{
		AimYawTarget   = YawRad;
		AimPitchTarget = PitchRad;
	}
}

void AGRNormalAICharacter::OnDead()
{
	if (IsValid(WeaponActor))
	{
		WeaponActor->Destroy();
	}

	Super::OnDead();
}

void AGRNormalAICharacter::OnRep_AimTarget()
{
}
