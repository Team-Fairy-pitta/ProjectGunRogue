#include "Character/GRCharacter.h"
#include "Character/GRInputHandleComponent.h"
#include "Player/GRPlayerController.h"
#include "Player/GRPlayerState.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"

AGRCharacter::AGRCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	InputHandleComponent = CreateDefaultSubobject<UGRInputHandleComponent>(TEXT("InputHandleComponent"));
}

void AGRCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AGRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (InputHandleComponent)
	{
		InputHandleComponent->SetupPlayerInputComponent(PlayerInputComponent);
	}
}

AGRPlayerController* AGRCharacter::GetGRPlayerController() const
{
	return GetController<AGRPlayerController>();
}


AGRPlayerState* AGRCharacter::GetGRPlayerState() const
{
	return GetPlayerState<AGRPlayerState>();
}

UGRAbilitySystemComponent* AGRCharacter::GetGRAbilitySystemComponent() const
{
	return Cast<UGRAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* AGRCharacter::GetAbilitySystemComponent() const
{
	AGRPlayerState* GRPlayerState = GetGRPlayerState();
	if (IsValid(GRPlayerState))
	{
		return GRPlayerState->GetAbilitySystemComponent();
	}
	else
	{
		return nullptr;
	}
}

void AGRCharacter::CallSpectateNextPlayer()
{
	SpectateNextPlayer();
}

void AGRCharacter::CallSpectatePreviousPlayer()
{
	SpectatePreviousPlayer();
}

void AGRCharacter::CallResetSpectatePlayer()
{
	ResetSpectatePlayer();
}
