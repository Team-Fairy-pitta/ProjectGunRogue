#include "Augment/GRAugmentSelector.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Character/GRCharacter.h"
#include "Player/GRPlayerState.h"
#include "Player/Battle/GRBattlePlayerController.h"
#include "Net/UnrealNetwork.h"


AGRAugmentSelector::AGRAugmentSelector()
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

void AGRAugmentSelector::BeginPlay()
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
			UE_LOG(LogTemp, Error, TEXT("[AGRAugmentSelector] InteractWidgetClass is INVALID"));
		}
	}
}

void AGRAugmentSelector::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, WasActivatedArray);
}

TArray<TObjectPtr<UStaticMeshComponent>> AGRAugmentSelector::GetMeshComponents()
{
	TArray<TObjectPtr<UStaticMeshComponent>> Meshes;
	Meshes.Add(StaticMeshComponent);
	return Meshes;
}

void AGRAugmentSelector::InteractWith(AActor* OtherActor)
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

	AGRPlayerState* GRPlayerState = GRCharacter->GetPlayerState<AGRPlayerState>();
	if (!IsValid(GRPlayerState))
	{
		UE_LOG(LogTemp, Error, TEXT("GRPlayerState (AGRPlayerState) is INVALID"));
		return;
	}

	AGRBattlePlayerController* BattlePlayerController = GRCharacter->GetController<AGRBattlePlayerController>();
	if (!IsValid(BattlePlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("BattlePlayerController is INVALID"));
		return;
	}

	if (WasActivatedArray.Contains(GRPlayerState))
	{
		UE_LOG(LogTemp, Display, TEXT("%s has activated this item box..."), *GRPlayerState->GetPlayerName());
		return;
	}

	DisableToSpecificPlayer(GRPlayerState);
	BattlePlayerController->ClientRPC_ShowAugmentWidget();
}

void AGRAugmentSelector::OnOver()
{
	if (InteractWidgetComponent)
	{
		InteractWidgetComponent->SetVisibility(true);
	}
}

void AGRAugmentSelector::OnOut()
{
	if (InteractWidgetComponent)
	{
		InteractWidgetComponent->SetVisibility(false);
	}
}

void AGRAugmentSelector::DisableToSpecificPlayer(AGRPlayerState* GRPlayerState)
{
	WasActivatedArray.Add(GRPlayerState);
}

bool AGRAugmentSelector::CanInteract(AActor* OtherActor)
{
	if (OtherActor->IsA<AGRCharacter>())
	{
		AGRCharacter* GRCharacter = Cast<AGRCharacter>(OtherActor);
		if (IsValid(GRCharacter))
		{
			AGRPlayerState* GRPlayerState = GRCharacter->GetGRPlayerState();
			if (IsValid(GRPlayerState))
			{
				bool bWasActivate = WasActivatedArray.Contains(GRPlayerState);
				return !bWasActivate;
			}
		}
	}
	else if (OtherActor->IsA<AGRPlayerState>())
	{
		AGRPlayerState* GRPlayerState = Cast<AGRPlayerState>(OtherActor);
		if (IsValid(GRPlayerState))
		{
			bool bWasActivate = WasActivatedArray.Contains(GRPlayerState);
			return !bWasActivate;
		}
	}
	return false;
}
