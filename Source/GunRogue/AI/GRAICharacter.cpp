// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GRAICharacter.h"
#include "GameFramework/CharacterMovementComponent.h" 

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
	}
	bUseControllerRotationYaw = false;

}


void AGRAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
}


