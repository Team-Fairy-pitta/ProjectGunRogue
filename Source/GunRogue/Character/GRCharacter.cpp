#include "Character/GRCharacter.h"
#include "Character/GRInputHandleComponent.h"
#include "Character/Interaction/GRInteractionComponent.h"
#include "Player/GRPlayerController.h"
#include "Player/GRPlayerState.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "AbilitySystem/Attributes/GRHealthAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"


AGRCharacter::AGRCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	InputHandleComponent = CreateDefaultSubobject<UGRInputHandleComponent>(TEXT("InputHandleComponent"));
	InteractionComponent = CreateDefaultSubobject<UGRInteractionComponent>(TEXT("GRInteractionComponent"));

	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	SpringArmComponent->SetupAttachment(RootComponent);
	SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	LastControllerRotation = FQuat::Identity;
	TargetRotation = FQuat::Identity;
}

void AGRCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	SetLastControllerRotation();
}

void AGRCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AGRCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (InputHandleComponent)
	{
		InputHandleComponent->SetupPlayerInputComponent(PlayerInputComponent);
	}
}

void AGRCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	ApplySmoothCameraControl(DeltaTime);
}

AGRPlayerController* AGRCharacter::GetGRPlayerController() const
{
	return GetController<AGRPlayerController>();
}


AGRPlayerState* AGRCharacter::GetGRPlayerState() const
{
	return GetPlayerState<AGRPlayerState>();
}

UGRAbilitySystemComponent* AGRCharacter::GetGRAbilitySystemComponent() const
{
	return Cast<UGRAbilitySystemComponent>(GetAbilitySystemComponent());
}

UAbilitySystemComponent* AGRCharacter::GetAbilitySystemComponent() const
{
	AGRPlayerState* GRPlayerState = GetGRPlayerState();
	if (IsValid(GRPlayerState))
	{
		return GRPlayerState->GetAbilitySystemComponent();
	}
	else
	{
		return nullptr;
	}
}

void AGRCharacter::CallSpectateNextPlayer()
{
	SpectateNextPlayer();
}

void AGRCharacter::CallSpectatePreviousPlayer()
{
	SpectatePreviousPlayer();
}

void AGRCharacter::CallResetSpectatePlayer()
{
	ResetSpectatePlayer();
}

bool AGRCharacter::IsTargetDead(ACharacter* TargetCharacter) const
{
	if (!IsValid(TargetCharacter))
	{
		return false;
	}
	const IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(TargetCharacter);
	const UAbilitySystemComponent* TargetASC = nullptr;

	if (ASCInterface)
	{
		TargetASC = ASCInterface->GetAbilitySystemComponent();
	}

	if (TargetASC)
	{
		if (const UGRHealthAttributeSet* HealthSet = Cast<UGRHealthAttributeSet>(TargetASC->GetAttributeSet(UGRHealthAttributeSet::StaticClass())))
		{
			return HealthSet->GetHealth() <= 0.0f;
		}
	}

	
	return false;
}

void AGRCharacter::SetLastControllerRotation()
{
	LastControllerRotation = GetControlRotation().Quaternion();
	TargetRotation = GetControlRotation().Quaternion();
}

void AGRCharacter::AddControllerYawSmooth_Temporal(float Value)
{
	FRotator AddedRotation(0.0f, Value, 0.0f);
	FQuat AddedQuat = AddedRotation.Quaternion();
	TargetRotation *= AddedQuat;
	TargetRotation.Normalize();
}

void AGRCharacter::AddControllerPitchSmooth_Temporal(float Value)
{
	FRotator AddedRotation(Value, 0.0f, 0.0f);
	FQuat AddedQuat = AddedRotation.Quaternion();
	TargetRotation *= AddedQuat;
	TargetRotation.Normalize();
}

void AGRCharacter::AddControllerYawSmooth(float Value)
{
	AddControllerYawSmooth_Temporal(Value);
	LastControllerRotation = TargetRotation;
}

void AGRCharacter::AddControllerPitchSmooth(float Value)
{
	AddControllerPitchSmooth_Temporal(Value);
	LastControllerRotation = TargetRotation;
}

void AGRCharacter::ReturnToLastControllerRotation()
{
	TargetRotation = LastControllerRotation;
	TargetRotation.Normalize();
}

void AGRCharacter::ApplySmoothCameraControl(float DeltaTime)
{
	FQuat Current = GetControlRotation().Quaternion();
	if (Current.Equals(TargetRotation))
	{
		return;
	}

	float Rate = SmoothAlpha * DeltaTime * SmoothSpeed;
	FQuat DeltaQuat = FQuat::Slerp(Current, TargetRotation, Rate);
	DeltaQuat.Normalize();
	FRotator DeltaRotator = DeltaQuat.Rotator();
	DeltaRotator.Roll = 0.0f;
	Controller->SetControlRotation(DeltaRotator);
}
