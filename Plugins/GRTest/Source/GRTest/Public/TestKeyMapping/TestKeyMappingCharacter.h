#pragma once

#include "Character/GRCharacter.h"
#include "TestKeyMappingCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

class UEnhancedInputLocalPlayerSubsystem;
class UEnhancedInputUserSettings;

UCLASS()
class GRTEST_API ATestKeyMappingCharacter : public AGRCharacter
{
	GENERATED_BODY()
	
public:
	ATestKeyMappingCharacter();
	virtual void BeginPlay() override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION()
	void OnInputMove(const FInputActionValue& InputActionValue);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> ForwardInputAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> BackwardInputAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> RightInputAction;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> LeftInputAction;

private:
	// 전부 테스트용 함수
	void RegisterInputMappingContext(UEnhancedInputUserSettings* UserSettings, UInputMappingContext* MappingContext);
	void GetAllSavedKeyProfiles(UEnhancedInputUserSettings* UserSettings);
	
	UEnhancedInputUserSettings* LoadOrCreateUserSettings(ULocalPlayer* LocalPlayer);
	void SaveInputUserSettings(UEnhancedInputUserSettings* UserSettings);


	void RandomMappingTest(UEnhancedInputUserSettings* UserSettings);
};
