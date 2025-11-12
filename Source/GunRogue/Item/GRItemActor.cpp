#include "Item/GRItemActor.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"

AGRItemActor::AGRItemActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(StaticMeshComponent);

	InteractionBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBoxComponent"));
	InteractionBoxComponent->SetupAttachment(StaticMeshComponent);
}

void AGRItemActor::BeginPlay()
{
	Super::BeginPlay();
	
}

TArray<TObjectPtr<UStaticMeshComponent>> AGRItemActor::GetMeshComponents()
{
	TArray<TObjectPtr<UStaticMeshComponent>> MeshComponents;
	MeshComponents.Add(StaticMeshComponent);
	return MeshComponents;
}
