#include "TestSample/TestActor.h"

ATestActor::ATestActor()
{
	PrimaryActorTick.bCanEverTick = true;
}

void ATestActor::BeginPlay()
{
	Super::BeginPlay();
}

void ATestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

