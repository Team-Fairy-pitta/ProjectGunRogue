#pragma once

#include "GameFramework/Character.h"
#include "GRCharacter.generated.h"

UCLASS()
class GUNROGUE_API AGRCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AGRCharacter();
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
