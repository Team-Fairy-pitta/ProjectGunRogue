#include "Character/GRInputHandleComponent.h"
#include "Character/GRCharacter.h"
#include "Character/GRPawnData.h"
#include "Input/GRInputComponent.h"
#include "Input/GRInputConfig.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "GameFramework/PlayerController.h"
#include "EnhancedInputSubsystems.h"


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
	
	APlayerController* PlayerController = GRCharacter->GetController<APlayerController>();
	check(PlayerController);

	ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	check(LocalPlayer);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	Subsystem->ClearAllMappings();

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
		FModifyContextOptions Options = {};
		Options.bIgnoreAllPressedKeysUntilRelease = false;
		Subsystem->AddMappingContext(Mapping.InputMappingContext, Mapping.Priority, Options);
	}

	TArray<uint32> OutBindHandles;
	GRInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, OutBindHandles);
	
	GRInputComponent->BindNativeAction(InputConfig, GunRogue::InputTag::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move);
	GRInputComponent->BindNativeAction(InputConfig, GunRogue::InputTag::InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse);
	GRInputComponent->BindNativeAction(InputConfig, GunRogue::InputTag::InputTag_Crouch, ETriggerEvent::Triggered, this, &ThisClass::Input_Crouch);
}

void UGRInputHandleComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	AGRCharacter* GRCharacter = GetOwningCharacter();
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
	AGRCharacter* GRCharacter = GetOwningCharacter();
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
}

void UGRInputHandleComponent::Input_LookMouse(const FInputActionValue& InputActionValue)
{
}

void UGRInputHandleComponent::Input_Crouch(const FInputActionValue& InputActionValue)
{
}

AGRCharacter* UGRInputHandleComponent::GetOwningCharacter()
{
	return GetOwner<AGRCharacter>();
}
