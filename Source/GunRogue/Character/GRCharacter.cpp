#include "Character/GRCharacter.h"

AGRCharacter::AGRCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AGRCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AGRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}
