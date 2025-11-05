// Fill out your copyright notice in the Description page of Project Settings.


#include "TestUI/TestUICharacter.h"
#include "EnhancedInputComponent.h"
#include "TestUI/TestUIPlayerController.h"

// Sets default values
ATestUICharacter::ATestUICharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void ATestUICharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATestUICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ATestUICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (UEnhancedInputComponent* EnhancedInput = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (ATestUIPlayerController* PC = Cast<ATestUIPlayerController>(GetController()))
		{
			if (PC->MoveAction)
			{
				EnhancedInput->BindAction(
					PC->MoveAction,
					ETriggerEvent::Triggered,
					this,
					&ATestUICharacter::Move
				);
			}
		}
	}
}

void ATestUICharacter::Move(const FInputActionValue& Value)
{
	if (!Controller) return;

	const FVector2D MoveInput = Value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}

	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}

