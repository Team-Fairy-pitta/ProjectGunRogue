#include "Item/GRItemBoxSpawnPoint.h"
#include "Components/BoxComponent.h"
#include "Components/ArrowComponent.h"

AGRItemBoxSpawnPoint::AGRItemBoxSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	BoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComponent"));
	BoxComponent->SetupAttachment(SceneRoot);

	ArrowComponent = CreateDefaultSubobject<UArrowComponent>(TEXT("ArrowComponent"));
	ArrowComponent->SetupAttachment(SceneRoot);
}

void AGRItemBoxSpawnPoint::BeginPlay()
{
	Super::BeginPlay();
}
