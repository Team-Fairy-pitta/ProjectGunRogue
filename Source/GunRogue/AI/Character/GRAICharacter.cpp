// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Character/GRAICharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AbilitySystemComponent.h"
#include "Components/CapsuleComponent.h"

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

	bReplicates = true;
	SetReplicateMovement(true);

	UCapsuleComponent* Capsule = GetCapsuleComponent();
	if (Capsule)
	{
		Capsule->BodyInstance.SetCollisionProfileName(TEXT("AIBoss"));
	}

	USkeletalMeshComponent* SkelMesh = GetMesh();
	if (SkelMesh)
	{
		SkelMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1,ECollisionResponse::ECR_Ignore);
	}
}

UAbilitySystemComponent* AGRAICharacter::GetAbilitySystemComponent() const
{
	return ASC;
}


void AGRAICharacter::BeginPlay()
{
	Super::BeginPlay();

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
		}	
	}
}


