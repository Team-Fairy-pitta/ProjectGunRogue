// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Character/GRCharacter.h"
#include "InputActionValue.h"
#include "TestGRCharacter_Cheat.generated.h"


class UCameraComponent;
class USpringArmComponent;
class UInputMappingContext;
class UInputAction;
/**
 * 
 */
UCLASS()
class GRTEST_API ATestGRCharacter_Cheat : public AGRCharacter
{
	GENERATED_BODY()

#pragma region ACharacter Override

public:
	ATestGRCharacter_Cheat();

	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;

	virtual void BeginPlay() override;

#pragma endregion

#pragma region Component
public:

	FORCEINLINE USpringArmComponent* GetSpringArm() const { return SpringArm; }

	FORCEINLINE UCameraComponent* GetCamera() const { return Camera; }

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AGXCharacter|Component")
	TObjectPtr<USpringArmComponent> SpringArm;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AGXCharacter|Component")
	TObjectPtr<UCameraComponent> Camera;
#pragma endregion

#pragma region Input
private:
	void HandleMoveInput(const FInputActionValue& InValue);

	void HandleLookInput(const FInputActionValue& InValue);

	void HandleJumpOrFlyInput(const FInputActionValue& InValue);
	void HandleFlyDownInput(const FInputActionValue& InValue);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AGXCharacter|Input")
	TObjectPtr<UInputMappingContext> InputMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AGXCharacter|Input")
	TObjectPtr<UInputAction> MoveAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AGXCharacter|Input")
	TObjectPtr<UInputAction> LookAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AGXCharacter|Input")
	TObjectPtr<UInputAction> JumpAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AGXCharacter|Input")
	TObjectPtr<UInputAction> DownAction;

#pragma endregion
};
