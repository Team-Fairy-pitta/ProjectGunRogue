#pragma once

#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GRCharacter.generated.h"

class AGRPlayerController;
class AGRPlayerState;
class UGRAbilitySystemComponent;
class UGRInputHandleComponent;
class UGRPawnData;

UCLASS()
class GUNROGUE_API AGRCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGRCharacter();
	virtual void BeginPlay() override;
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
};
