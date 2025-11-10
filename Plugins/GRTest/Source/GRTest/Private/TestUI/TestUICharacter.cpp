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

			if (PC->SkillAction_Q)
			{
				EnhancedInput->BindAction(
					PC->SkillAction_Q,
					ETriggerEvent::Triggered,
					this,
					&ATestUICharacter::Skill_Q
				);
			}

			if (PC->SkillAction_E)
			{
				EnhancedInput->BindAction(
					PC->SkillAction_E,
					ETriggerEvent::Triggered,
					this,
					&ATestUICharacter::Skill_E
				);
			}

			if (PC->SkillAction_Shift)
			{
				EnhancedInput->BindAction(
					PC->SkillAction_Shift,
					ETriggerEvent::Triggered,
					this,
					&ATestUICharacter::Skill_Shift
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

void ATestUICharacter::Skill_Q(const FInputActionValue& Value)
{
	if (ATestUIPlayerController* PC = Cast<ATestUIPlayerController>(GetController()))
	{
		PC->HandleSkillInput("Q");
	}
}

void ATestUICharacter::Skill_E(const FInputActionValue& Value)
{
	if (ATestUIPlayerController* PC = Cast<ATestUIPlayerController>(GetController()))
	{
		PC->HandleSkillInput("E");
	}
}

void ATestUICharacter::Skill_Shift(const FInputActionValue& Value)
{
	if (ATestUIPlayerController* PC = Cast<ATestUIPlayerController>(GetController()))
	{
		PC->HandleSkillInput("Shift");
	}
}


