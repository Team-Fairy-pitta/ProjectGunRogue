#include "System/GRLevel1ControlPanel.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Character/GRCharacter.h"
#include "Player/Battle/GRBattlePlayerController.h"
#include "Net/UnrealNetwork.h"

AGRLevel1ControlPanel::AGRLevel1ControlPanel()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(StaticMeshComponent);

	StaticMeshComponent_Sub1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent_Sub1"));
	StaticMeshComponent_Sub1->SetupAttachment(StaticMeshComponent);

	StaticMeshComponent_Sub2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent_Sub2"));
	StaticMeshComponent_Sub2->SetupAttachment(StaticMeshComponent);

	StaticMeshComponent_Sub3 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent_Sub3"));
	StaticMeshComponent_Sub3->SetupAttachment(StaticMeshComponent);

	InteractWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractWidgetComponent"));
	InteractWidgetComponent->SetupAttachment(StaticMeshComponent);
	InteractWidgetComponent->SetWidgetSpace(EWidgetSpace::Screen);
	InteractWidgetComponent->SetVisibility(false);
	InteractWidgetComponent->SetDrawSize(FVector2D(300, 100)); // Desired Size of UUserWidget

	bWasActivated = false;
}

void AGRLevel1ControlPanel::BeginPlay()
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

void AGRLevel1ControlPanel::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, bWasActivated);
}

TArray<TObjectPtr<UStaticMeshComponent>> AGRLevel1ControlPanel::GetMeshComponents()
{
	TArray<TObjectPtr<UStaticMeshComponent>> MeshComponents;
	MeshComponents.Add(StaticMeshComponent);
	MeshComponents.Add(StaticMeshComponent_Sub1);
	MeshComponents.Add(StaticMeshComponent_Sub2);
	MeshComponents.Add(StaticMeshComponent_Sub3);
	return MeshComponents;
}

void AGRLevel1ControlPanel::InteractWith(AActor* OtherActor)
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

	if (bWasActivated)
	{
		UE_LOG(LogTemp, Display, TEXT("bWasActivated == true"));
		return;
	}

	BattlePC->ClientRPC_ShowLevel1SelectWidget();
}

void AGRLevel1ControlPanel::OnOver()
{
	if (InteractWidgetComponent)
	{
		InteractWidgetComponent->SetVisibility(true);
	}
}

void AGRLevel1ControlPanel::OnOut()
{
	if (InteractWidgetComponent)
	{
		InteractWidgetComponent->SetVisibility(false);
	}
}

bool AGRLevel1ControlPanel::CanInteract(AActor* OtherActor)
{
	if (bWasActivated)
	{
		return false;
	}
	else
	{
		return true;
	}
}
