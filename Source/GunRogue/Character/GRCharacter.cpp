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

#include "Weapon/GRWeaponBase.h"
#include "Net/UnrealNetwork.h"



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
}

void AGRCharacter::BeginPlay()
{
	Super::BeginPlay();
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

void AGRCharacter::Test_EquipWeapon()
{
	if (!Weapon)
	{
		return;
	}

	if (HasAuthority())
	{
		Server_EquipWeapon(); 
	}
	else
	{
		Server_EquipWeapon();
	}
}

void AGRCharacter::Server_EquipWeapon_Implementation()
{
	if (!Weapon)
	{
		return;
	}

	Weapon->EquipWeapon(GetAbilitySystemComponent());
}

void AGRCharacter::Test_UpgradeWeapon()
{
	if (!Weapon)
	{
		return;
	}

	if (HasAuthority())
	{
		Server_UpgradeWeapon();
	}
	else
	{
		Server_UpgradeWeapon();
	}
}

void AGRCharacter::Server_UpgradeWeapon_Implementation()
{
	if (!Weapon)
	{
		return;
	}

	Weapon->TryUpgradeWeapon();
}

void AGRCharacter::Test_UnequipWeapon()
{
	if (!Weapon)
	{
		return;
	}

	if (HasAuthority())
	{
		Server_UnequipWeapon();
	}
	else
	{
		Server_UnequipWeapon();
	}
}

void AGRCharacter::Server_UnequipWeapon_Implementation()
{
	if (!Weapon)
	{
		return;
	}

	Weapon->UnequipWeapon();
	Weapon = nullptr;
}

AGRWeaponBase* AGRCharacter::GetWeapon() const
{
	return Weapon;
}

void AGRCharacter::NotifyWeaponOverlap(AGRWeaponBase* GetWeapon)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!GetWeapon)
	{
		return;
	}

	Weapon = GetWeapon;

	Weapon->SetOwner(this);

	UE_LOG(LogTemp, Warning, TEXT("무기 감지: %s"), *Weapon->GetName());
}

void AGRCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AGRCharacter, Weapon);
}
