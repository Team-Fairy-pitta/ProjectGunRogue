#include "Character/GRInputHandleComponent.h"
#include "Character/GRCharacter.h"
#include "Character/GRPawnData.h"
#include "Player/GRPlayerController.h"
#include "Input/GRInputComponent.h"
#include "Input/GRInputConfig.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "UserSettings/EnhancedInputUserSettings.h"


namespace GunRogue::InputTag
{
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Move, "InputTag.Move", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Look_Mouse, "InputTag.Look_Mouse", "");
	UE_DEFINE_GAMEPLAY_TAG_COMMENT(InputTag_Crouch, "InputTag.Crouch", "");

	UE_DEFINE_GAMEPLAY_TAG_COMMENT(Ability_InputBlocked, "Ability.InputBlocked", "");
}

UGRInputHandleComponent::UGRInputHandleComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGRInputHandleComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UGRInputHandleComponent::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	UGRInputComponent* GRInputComponent = Cast<UGRInputComponent>(PlayerInputComponent);
	check(GRInputComponent);

	AGRCharacter* GRCharacter = GetOwner<AGRCharacter>();
	check(GRCharacter);
	
	AGRPlayerController* GRPlayerController = GRCharacter->GetController<AGRPlayerController>();
	check(GRPlayerController);

	ULocalPlayer* LocalPlayer = GRPlayerController->GetLocalPlayer();
	check(LocalPlayer);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();

	UEnhancedInputUserSettings* UserSettings = UEnhancedInputUserSettings::LoadOrCreateSettings(LocalPlayer);
	if (!UserSettings)
	{
		UE_LOG(LogTemp, Error, TEXT("UsetSettings (UEnhancedInputUserSettings) is INVALID."));
		return;
	}

	for (UInputMappingContext* MappableContext : GRPlayerController->MappableMappingContexts)
	{
		UserSettings->RegisterInputMappingContext(MappableContext);
	}

	const UGRPawnData* PawnData = GRCharacter->GetPawnData();
	if (!PawnData)
	{
		return;
	}


	const UGRInputConfig* InputConfig = PawnData->InputConfig;
	if (!InputConfig)
	{
		return;
	}

	for (const FInputMappintContextAndPriority& Mapping : InputConfig->InputMappings)
	{
		if (Mapping.InputMappingContext)
		{
			FModifyContextOptions Options = {};
			Options.bIgnoreAllPressedKeysUntilRelease = false;
			Subsystem->AddMappingContext(Mapping.InputMappingContext, Mapping.Priority, Options);
		}
	}

	UserSettings->SaveSettings();

	TArray<uint32> OutBindHandles;
	GRInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, OutBindHandles);
	
	GRInputComponent->BindNativeAction(InputConfig, GunRogue::InputTag::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
	GRInputComponent->BindNativeAction(InputConfig, GunRogue::InputTag::InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse);
	GRInputComponent->BindNativeAction(InputConfig, GunRogue::InputTag::InputTag_Crouch, ETriggerEvent::Triggered, this, &ThisClass::Input_Crouch);
}

void UGRInputHandleComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	AGRCharacter* GRCharacter = GetOwnerCharacter();
	if (!IsValid(GRCharacter))
	{
		return;
	}
	
	UGRAbilitySystemComponent* ASC = GRCharacter->GetGRAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}
	
	ASC->AbilityInputTagPressed(InputTag);
}

void UGRInputHandleComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	AGRCharacter* GRCharacter = GetOwnerCharacter();
	if (!IsValid(GRCharacter))
	{
		return;
	}

	UGRAbilitySystemComponent* ASC = GRCharacter->GetGRAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}

	ASC->AbilityInputTagReleased(InputTag);
}

void UGRInputHandleComponent::Input_Move(const FInputActionValue& InputActionValue)
{
	ACharacter* Character = GetOwnerCharacter();
	AController* Controller = Character ? Character->GetController() : nullptr;

	if (IsValid(Controller))
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();
		const FRotator MovementRotation(0.0f, Controller->GetControlRotation().Yaw, 0.0f);

		if (!FMath::IsNearlyZero(Value.X))
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::ForwardVector);
			Character->AddMovementInput(MovementDirection, Value.X);
		}
		if (!FMath::IsNearlyZero(Value.Y))
		{
			const FVector MovementDirection = MovementRotation.RotateVector(FVector::RightVector);
			Character->AddMovementInput(MovementDirection, Value.Y);
		}
	}
}

void UGRInputHandleComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
	ACharacter* Character = GetOwnerCharacter();

	if (IsValid(Character))
	{
		const FVector2D Value = InputActionValue.Get<FVector2D>();

		if (!FMath::IsNearlyZero(Value.X))
		{
			Character->AddControllerYawInput(Value.X);
		}
		if (!FMath::IsNearlyZero(Value.Y))
		{
			Character->AddControllerPitchInput(Value.Y);
		}
	}
}

void UGRInputHandleComponent::Input_Crouch(const FInputActionValue& InputActionValue)
{
	ACharacter* Character = GetOwnerCharacter();

	if (IsValid(Character))
	{
		if (Character->bIsCrouched)
		{
			Character->UnCrouch();
		}
		else
		{
			Character->Crouch();
		}
	}
}

AGRCharacter* UGRInputHandleComponent::GetOwnerCharacter()
{
	return GetOwner<AGRCharacter>();
}
