#include "System/GRLevel1ControlPanel.h"
#include "Components/StaticMeshComponent.h"
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

	bWasActivated = false;
}

void AGRLevel1ControlPanel::BeginPlay()
{
	Super::BeginPlay();
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
}

void AGRLevel1ControlPanel::OnOver()
{
}

void AGRLevel1ControlPanel::OnOut()
{
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
