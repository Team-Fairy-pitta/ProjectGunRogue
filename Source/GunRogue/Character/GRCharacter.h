#pragma once

#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GRCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UBoxComponent;
class AGRPlayerController;
class AGRPlayerState;
class UGRAbilitySystemComponent;
class UGRInputHandleComponent;
class UGRInteractionComponent;
class UGRPawnData;

UCLASS()
class GUNROGUE_API AGRCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGRCharacter();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "AITCharacter")
	AGRPlayerController* GetGRPlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "AITCharacter")
	AGRPlayerState* GetGRPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "AITCharacter")
	virtual UGRAbilitySystemComponent* GetGRAbilitySystemComponent() const;

	const UGRPawnData* GetPawnData() const { return PawnData; }
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn Data")
	TObjectPtr<UGRPawnData> PawnData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UGRInputHandleComponent> InputHandleComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UGRInteractionComponent> InteractionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> CameraComponent;
};
