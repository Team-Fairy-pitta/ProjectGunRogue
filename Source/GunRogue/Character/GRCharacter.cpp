#include "Character/GRCharacter.h"
#include "Character/GRInputHandleComponent.h"
#include "Character/Interaction/GRInteractionComponent.h"
#include "Character/Attachment/GRAttachmentComponent.h"
#include "Player/GRPlayerController.h"
#include "Player/GRPlayerState.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "AbilitySystem/Attributes/GRHealthAttributeSet.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "MiniMap/GRRadarMapComponent.h"
#include "Net/UnrealNetwork.h"

AGRCharacter::AGRCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	InputHandleComponent = CreateDefaultSubobject<UGRInputHandleComponent>(TEXT("InputHandleComponent"));
	InteractionComponent = CreateDefaultSubobject<UGRInteractionComponent>(TEXT("GRInteractionComponent"));
	AttachmentComponent = CreateDefaultSubobject<UGRAttachmentComponent>(TEXT("AttachmentComponent"));
	RadarMapComponent = CreateDefaultSubobject<UGRRadarMapComponent>(TEXT("RadarMapComponent"));

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

USkeletalMeshComponent* AGRCharacter::GetEquippedWeaponMesh() const
{
	// 캐릭터에 붙어있는 모든 ChildActorComponent 찾기
	TArray<UChildActorComponent*> ChildActorComponents;
	GetComponents<UChildActorComponent>(ChildActorComponents);

	// 메시에 Muzzle 소켓이 포함되었으면 무기로 간주
	TArray<FName> MuzzleSocketNames = {
		FName("Muzzle"),
		FName("muzzle")
	};

	for (UChildActorComponent* ChildComp : ChildActorComponents)
	{
		if (!ChildComp)
		{
			continue;
		}

		AActor* ChildActor = ChildComp->GetChildActor();
		if (!ChildActor)
		{
			continue;
		}

		// ChildActor에서 SkeletalMeshComponent 찾기
		USkeletalMeshComponent* WeaponMesh = ChildActor->FindComponentByClass<USkeletalMeshComponent>();
		if (WeaponMesh)
		{
			// Muzzle 소켓이 있는지 확인
			for (const FName& SocketName : MuzzleSocketNames)
			{
				if (WeaponMesh->DoesSocketExist(SocketName))
				{
					// Muzzle 소켓이 있는 메시만 반환
					return WeaponMesh;
				}
			}
		}
	}

	return nullptr;
}

UStaticMeshComponent* AGRCharacter::GetEquippedWeaponStaticMesh() const
{
	// 캐릭터에 붙어있는 모든 ChildActorComponent 찾기
	TArray<UChildActorComponent*> ChildActorComponents;
	GetComponents<UChildActorComponent>(ChildActorComponents);

	// 메시에 Muzzle 소켓이 포함되었으면 무기로 간주
	TArray<FName> MuzzleSocketNames = {
		FName("Muzzle"),
		FName("muzzle")
	};

	for (UChildActorComponent* ChildComp : ChildActorComponents)
	{
		if (!ChildComp)
		{
			continue;
		}

		AActor* ChildActor = ChildComp->GetChildActor();
		if (!ChildActor)
		{
			continue;
		}

		// ChildActor에서 StaticMeshComponent 찾기
		UStaticMeshComponent* WeaponMesh = ChildActor->FindComponentByClass<UStaticMeshComponent>();
		if (WeaponMesh)
		{
			// Muzzle 소켓이 있는지 확인
			for (const FName& SocketName : MuzzleSocketNames)
			{
				if (WeaponMesh->DoesSocketExist(SocketName))
				{
					// Muzzle 소켓이 있는 메시만 반환
					return WeaponMesh;
				}
			}
		}
	}

	return nullptr;
}
