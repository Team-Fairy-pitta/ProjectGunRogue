#include "TestKeyMapping/TestKeyMappingCharacter.h"
#include "TestKeyMapping/TestKeyMappingPC.h"
#include "GunRogue/Input/GRInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "UserSettings/EnhancedInputUserSettings.h"

#include "Math/UnrealMathUtility.h"

ATestKeyMappingCharacter::ATestKeyMappingCharacter()
{
}

void ATestKeyMappingCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void ATestKeyMappingCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// 테스트를 위해, 부모의 SetupPlayerInputComponent()를 사용하지 않음
	// Super::SetupPlayerInputComponent(PlayerInputComponent);

	UGRInputComponent* GRInputComponent = Cast<UGRInputComponent>(PlayerInputComponent);
	check(GRInputComponent);

	ATestKeyMappingPC* PlayerController = GetController<ATestKeyMappingPC>();
	check(PlayerController);

	ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	check(LocalPlayer);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();

	UEnhancedInputUserSettings* UserSettings = UEnhancedInputUserSettings::LoadOrCreateSettings(LocalPlayer);
	check(UserSettings);

	if (DefaultMappingContext)
	{
		UserSettings->RegisterInputMappingContext(DefaultMappingContext);

		float Priority = 0.0f;
		Subsystem->AddMappingContext(DefaultMappingContext, Priority);
	}
	if (ForwardInputAction)
	{
		GRInputComponent->BindAction(ForwardInputAction, ETriggerEvent::Triggered, this, &ThisClass::OnInputMove);
	}
	if (BackwardInputAction)
	{
		GRInputComponent->BindAction(BackwardInputAction, ETriggerEvent::Triggered, this, &ThisClass::OnInputMove);
	}
	if (RightInputAction)
	{
		GRInputComponent->BindAction(RightInputAction, ETriggerEvent::Triggered, this, &ThisClass::OnInputMove);
	}
	if (LeftInputAction)
	{
		GRInputComponent->BindAction(LeftInputAction, ETriggerEvent::Triggered, this, &ThisClass::OnInputMove);
	}
}

void ATestKeyMappingCharacter::OnInputMove(const FInputActionValue& InputActionValue)
{
	if (IsValid(Controller))
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (!FMath::IsNearlyZero(Value.X))
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			AddMovementInput(MovementDirection, Value.X);
		}
		if (!FMath::IsNearlyZero(Value.Y))
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			AddMovementInput(MovementDirection, Value.Y);
		}
	}
}
