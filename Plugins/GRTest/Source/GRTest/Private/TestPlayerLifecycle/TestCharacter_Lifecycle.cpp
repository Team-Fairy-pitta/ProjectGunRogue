// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerLifecycle/TestCharacter_Lifecycle.h"

// Sets default values
ATestCharacter_Lifecycle::ATestCharacter_Lifecycle()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ATestCharacter_Lifecycle::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATestCharacter_Lifecycle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATestCharacter_Lifecycle::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

