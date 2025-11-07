// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GRAICharacter.h"
#include "AI/GRAIController.h"

AGRAICharacter::AGRAICharacter()
{
	PrimaryActorTick.bCanEverTick = false;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned; 
}


void AGRAICharacter::BeginPlay()
{
	Super::BeginPlay();

	AGRAIController* AIController = Cast<AGRAIController>(GetController());
    
	if (AIController)
	{
		UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
	}
}


