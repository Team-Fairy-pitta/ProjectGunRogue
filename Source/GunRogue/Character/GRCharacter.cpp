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
	SpringArmComponent->SetRelativeLocation(ThirdPerson_CameraArmLocation);
	SpringArmComponent->TargetArmLength = ThirdPerson_CameraArmLength;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(SpringArmComponent);

	LastControllerRotation = FQuat::Identity;
	TargetCameraRotation = FQuat::Identity;
}

void AGRCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	SetLastControllerRotation();
	SetThirdPersonViewSmooth();
	bIsCameraAttachedToHead = false;
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

	ApplySmoothCameraControl_Rotation(DeltaTime);
	ApplySmoothCameraControl_CameraArm(DeltaTime);
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

#pragma region SmoothCameraControl

void AGRCharacter::SetLastControllerRotation()
{
	LastControllerRotation = GetControlRotation().Quaternion();
	TargetCameraRotation = GetControlRotation().Quaternion();
}

void AGRCharacter::AddControllerYawSmooth_Temporal(float Value)
{
	FRotator AddedRotation(0.0f, Value, 0.0f);
	FQuat AddedQuat = AddedRotation.Quaternion();
	TargetCameraRotation *= AddedQuat;
	TargetCameraRotation.Normalize();
}

void AGRCharacter::AddControllerPitchSmooth_Temporal(float Value)
{
	FRotator AddedRotation(Value, 0.0f, 0.0f);
	FQuat AddedQuat = AddedRotation.Quaternion();
	TargetCameraRotation *= AddedQuat;
	TargetCameraRotation.Normalize();
}

void AGRCharacter::AddControllerYawSmooth(float Value)
{
	AddControllerYawSmooth_Temporal(Value);
	LastControllerRotation = TargetCameraRotation;
}

void AGRCharacter::AddControllerPitchSmooth(float Value)
{
	AddControllerPitchSmooth_Temporal(Value);
	LastControllerRotation = TargetCameraRotation;
}

void AGRCharacter::ReturnToLastControllerRotation()
{
	TargetCameraRotation = LastControllerRotation;
	TargetCameraRotation.Normalize();
}

void AGRCharacter::SetFirstPersonViewSmooth()
{
	TargetCameraArmLocation = FirstPerson_CameraArmLocation;
	TargetCameraArmLength = FirstPerson_CameraArmLength;
}

void AGRCharacter::SetThirdPersonViewSmooth()
{
	TargetCameraArmLocation = ThirdPerson_CameraArmLocation;
	TargetCameraArmLength = ThirdPerson_CameraArmLength;
}

void AGRCharacter::ApplySmoothCameraControl_Rotation(float DeltaTime)
{
	FQuat Current = GetControlRotation().Quaternion();
	if (Current.Equals(TargetCameraRotation))
	{
		return;
	}

	float Rate = SmoothAlpha * DeltaTime * SmoothSpeed;
	FQuat DeltaQuat = FQuat::Slerp(Current, TargetCameraRotation, Rate);
	DeltaQuat.Normalize();
	FRotator DeltaRotator = DeltaQuat.Rotator();
	DeltaRotator.Roll = 0.0f;
	Controller->SetControlRotation(DeltaRotator);
}

void AGRCharacter::ApplySmoothCameraControl_CameraArm(float DeltaTime)
{
	if (!SpringArmComponent)
	{
		return;
	}
	float Rate = SmoothAlpha * DeltaTime * SmoothSpeed;

	float CurrentArmLength = SpringArmComponent->TargetArmLength;
	if (!FMath::IsNearlyEqual(TargetCameraArmLength, CurrentArmLength))
	{
		float Delta = FMath::Lerp(CurrentArmLength, TargetCameraArmLength, Rate);
		SpringArmComponent->TargetArmLength = Delta;

		/* Magic Number: 적당히 떨어져 있으면 캐릭터 표시 */
		if (SpringArmComponent->TargetArmLength >= 100)
		{
			SetCharacterVisibilityForCamera(true);
		}
		/* Magic Number: 1인칭 시점에서 캐릭터 표시 */
		else if (SpringArmComponent->TargetArmLength <= 5)
		{
			SetCharacterVisibilityForCamera(true);
		}
		/* Magic Number: 캐릭터를 통과할 때, 캐릭터가 보이지 않게 하기 */
		else
		{
			SetCharacterVisibilityForCamera(false);
		}

		/* Magic Number: 이 시점에서 전환하는 것이 자연스러움 */
		if (SpringArmComponent->TargetArmLength <= 50)
		{
			if (!bIsCameraAttachedToHead)
			{
				AttachCameraArmToHead();
			}
		}
		else
		{
			if (bIsCameraAttachedToHead)
			{
				AttachCameraArmToCapsule();
			}
		}
	}

	FVector CurrentArmLocation = SpringArmComponent->GetRelativeLocation();
	if (!(TargetCameraArmLocation - CurrentArmLocation).IsNearlyZero())
	{
		FVector Delta = FMath::Lerp(CurrentArmLocation, TargetCameraArmLocation, Rate);
		SpringArmComponent->SetRelativeLocation(Delta);
	}
}

void AGRCharacter::SetCharacterVisibilityForCamera(bool InVisibility)
{
	if (GetMesh())
	{
		GetMesh()->SetOwnerNoSee(InVisibility == false);
	}
}

void AGRCharacter::AttachCameraArmToCapsule()
{
	if (SpringArmComponent)
	{
		SpringArmComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
		bIsCameraAttachedToHead = false;
	}
}

void AGRCharacter::AttachCameraArmToHead()
{
	if (SpringArmComponent && GetMesh())
	{
		FName SocketName(TEXT("J_Bip_C_Head"));
		SpringArmComponent->AttachToComponent(GetMesh(), FAttachmentTransformRules::KeepRelativeTransform, SocketName);
		bIsCameraAttachedToHead = true;
	}
}

#pragma endregion SmoothCameraControl
