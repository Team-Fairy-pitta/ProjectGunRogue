#include "Weapon/GRWeaponUpgradeConsole.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Character/GRCharacter.h"
#include "Player/Battle/GRBattlePlayerController.h"
#include "Net/UnrealNetwork.h"

AGRWeaponUpgradeConsole::AGRWeaponUpgradeConsole()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(StaticMeshComponent);

	InteractWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidgetComponent"));
	InteractWidgetComponent->SetupAttachment(StaticMeshComponent);
	InteractWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	InteractWidgetComponent->SetVisibility(false);
	InteractWidgetComponent->SetDrawSize(FVector2D(300, 100)); // Desired Size of UUserWidget
	
}

void AGRWeaponUpgradeConsole::BeginPlay()
{
	Super::BeginPlay();

	if (IsValid(InteractWidgetComponent))
	{
		if (InteractWidgetClass)
		{
			InteractWidgetComponent->SetWidgetClass(InteractWidgetClass);
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("InteractWidgetClass is INVALID"));
		}
	}
}

void AGRWeaponUpgradeConsole::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

TArray<TObjectPtr<UStaticMeshComponent>> AGRWeaponUpgradeConsole::GetMeshComponents()
{
	TArray<TObjectPtr<UStaticMeshComponent>> MeshComponents;
	MeshComponents.Add(StaticMeshComponent);
	return MeshComponents;
}

void AGRWeaponUpgradeConsole::InteractWith(AActor* OtherActor)
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("InteractWith() REQUIRES authority"));
		return;
	}

	AGRCharacter* GRCharacter = Cast<AGRCharacter>(OtherActor);
	if (!IsValid(GRCharacter))
	{
		UE_LOG(LogTemp, Error, TEXT("GRCharacter (AGRCharacter) is INVALID"));
		return;
	}

	AGRBattlePlayerController* BattlePC = GRCharacter->GetController<AGRBattlePlayerController>();
	if (!IsValid(BattlePC))
	{
		UE_LOG(LogTemp, Error, TEXT("BattlePC (AGRBattlePlayerController) is INVALID"));
		return;
	}

	BattlePC->ClientRPC_ShowUpgradeConsoleWidget();
}

void AGRWeaponUpgradeConsole::OnOver()
{
	if (InteractWidgetComponent)
	{
		InteractWidgetComponent->SetVisibility(true);
	}
}

void AGRWeaponUpgradeConsole::OnOut()
{
	if (InteractWidgetComponent)
	{
		InteractWidgetComponent->SetVisibility(false);
	}
}

bool AGRWeaponUpgradeConsole::CanInteract(AActor* OtherActor)
{
	return true;
}
