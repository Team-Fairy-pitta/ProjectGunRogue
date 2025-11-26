// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GRAICharacter.h"
#include "GameFramework/CharacterMovementComponent.h"
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
}

UAbilitySystemComponent* AGRAICharacter::GetAbilitySystemComponent() const
{
	return ASC;
}


void AGRAICharacter::BeginPlay()
{
	Super::BeginPlay();
}


