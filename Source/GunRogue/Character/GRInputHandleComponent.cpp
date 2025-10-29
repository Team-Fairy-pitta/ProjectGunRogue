#include "Character/GRInputHandleComponent.h"


UGRInputHandleComponent::UGRInputHandleComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGRInputHandleComponent::BeginPlay()
{
	Super::BeginPlay();
}
