// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Character/GRAICharacter.h"
#include "Character/GRZLocationComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystem/Attributes/GRHealthAttributeSet.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"
#include "GameModes/Level1/GRGameMode_Level1.h"
#include "AbilitySystemComponent.h"

AGRAICharacter::AGRAICharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	UCharacterMovementComponent* MoveComp = GetCharacterMovement();
	if (MoveComp)
	{
		MoveComp->bUseControllerDesiredRotation = true;
		MoveComp->bOrientRotationToMovement = false;

		MoveComp->bUseRVOAvoidance = true;
		MoveComp->AvoidanceConsiderationRadius = 100.f;
		MoveComp->AvoidanceWeight = 0.5f;

		MoveComp->bRequestedMoveUseAcceleration = true;
	}
	bUseControllerRotationYaw = false;

	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComp"));
	ASC->SetIsReplicated(true);
	ASC->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	SetNetUpdateFrequency(100.0f);

	ZLocationComponent = CreateDefaultSubobject<UGRZLocationComponent>(TEXT("ZLocationComponent"));

	HealthAttributeSet = CreateDefaultSubobject<UGRHealthAttributeSet>(TEXT("HealthAttributeSet"));
	CombatAttributeSet = CreateDefaultSubobject<UGRCombatAttributeSet>(TEXT("CombatAttributeSet"));

	USkeletalMeshComponent* SkelMesh = GetMesh();
	if (SkelMesh)
	{
		SkelMesh->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	}
}

UAbilitySystemComponent* AGRAICharacter::GetAbilitySystemComponent() const
{
	return ASC;
}


void AGRAICharacter::BeginPlay()
{
	Super::BeginPlay();

	InitAbilitySystemComponent();
	NotifySpawnToGameMode();
}

void AGRAICharacter::EndPlay(EEndPlayReason::Type EndPlayReapon)
{
	Super::EndPlay(EndPlayReapon);

	NotifyDestroyToGameMode();
}

void AGRAICharacter::InitAbilitySystemComponent()
{
	if (HasAuthority())
	{
		if (ASC)
		{
			ASC->InitAbilityActorInfo(this, this);

			for (auto& AbilityClass : AttackAbilityClassList)
			{
				if (AbilityClass)
				{
					FGameplayAbilitySpec Spec(AbilityClass, /*Level*/1, /*InputID*/0, this);
					ASC->GiveAbility(Spec);
				}
			}

			ASC->GetGameplayAttributeValueChangeDelegate(UGRHealthAttributeSet::GetHealthAttribute()).AddUObject(this, &ThisClass::OnHealthChanged);
		}
	}
}

void AGRAICharacter::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	float Health = Data.NewValue;
	if (Health <= 0)
	{
		OnDead();
	}
}

void AGRAICharacter::OnDead()
{
	// [NOTE] TODO: 나중에 죽는 애니메이션 재생 등의 처리
	// 지금은 간단하게 actor 제거
	Destroy();
}

void AGRAICharacter::NotifySpawnToGameMode()
{
	if (!HasAuthority())
	{
		return;
	}
	if (!GetWorld())
	{
		return;
	}

	AGRGameMode_Level1* Level1GameMode = GetWorld()->GetAuthGameMode<AGRGameMode_Level1>();
	if (!IsValid(Level1GameMode))
	{
		UE_LOG(LogTemp, Error, TEXT("InteractWidgetClass is INVALID"));
		return;
	}

	Level1GameMode->ReceiveSpawnEnemy();
}

void AGRAICharacter::NotifyDestroyToGameMode()
{
	if (!HasAuthority())
	{
		return;
	}
	if (!GetWorld())
	{
		return;
	}

	AGRGameMode_Level1* Level1GameMode = GetWorld()->GetAuthGameMode<AGRGameMode_Level1>();
	if (!IsValid(Level1GameMode))
	{
		UE_LOG(LogTemp, Error, TEXT("InteractWidgetClass is INVALID"));
		return;
	}

	Level1GameMode->ReceiveDestroyEnemy();
}



