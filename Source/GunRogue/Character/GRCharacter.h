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

	// 블루프린트 구현 전용
	UFUNCTION(BlueprintImplementableEvent, Category = "Spectate")
	void SpectateNextPlayer();
	// 블루프린트 구현 전용
	UFUNCTION(BlueprintImplementableEvent, Category = "Spectate")
	void SpectatePreviousPlayer();
	// 블루프린트 구현 전용
	UFUNCTION(BlueprintImplementableEvent, Category = "Spectate")
	void ResetSpectatePlayer();
	
	// 테스트용 함수
	UFUNCTION(BlueprintCallable, Category = "Spectate")
	void CallSpectateNextPlayer();
	// 테스트용 함수
	UFUNCTION(BlueprintCallable, Category = "Spectate")
	void CallSpectatePreviousPlayer();
	// 테스트용 함수
	UFUNCTION(BlueprintCallable, Category = "Spectate")
	void CallResetSpectatePlayer();
	
};
