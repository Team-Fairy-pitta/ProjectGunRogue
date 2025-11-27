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
	virtual void Tick(float DeltaTime) override;

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
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Spectate")
	void SpectateNextPlayer();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Spectate")
	void SpectatePreviousPlayer();
	
	UFUNCTION(BlueprintImplementableEvent, Category = "Spectate")
	void ResetSpectatePlayer();
	
	UFUNCTION(BlueprintCallable, Category = "Spectate")
	void CallSpectateNextPlayer();
	
	UFUNCTION(BlueprintCallable, Category = "Spectate")
	void CallSpectatePreviousPlayer();
	
	UFUNCTION(BlueprintCallable, Category = "Spectate")
	void CallResetSpectatePlayer();

	UFUNCTION(BlueprintCallable, Category = "Spectate")
	bool IsTargetDead(ACharacter* TargetCharacter) const;


#pragma region SmoothCameraControl
public:
	void SetLastControllerRotation();

	UFUNCTION(BlueprintCallable)
	void AddControllerYawSmooth_Temporal(float Value);

	UFUNCTION(BlueprintCallable)
	void AddControllerPitchSmooth_Temporal(float Value);

	UFUNCTION(BlueprintCallable)
	void AddControllerYawSmooth(float Value);

	UFUNCTION(BlueprintCallable)
	void AddControllerPitchSmooth(float Value);

	UFUNCTION(BlueprintCallable)
	void ReturnToLastControllerRotation();

protected:
	void ApplySmoothCameraControl(float DeltaTime);

	// 플레이어가 지정한 최근(Last) 컨트롤러 방향을 저장
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FQuat LastControllerRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunRogue|CameraControl")
	float SmoothAlpha = 0.75f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunRogue|CameraControl")
	float SmoothSpeed = 15.0f;

	FQuat TargetRotation;

#pragma endregion SmoothCameraControl
};
