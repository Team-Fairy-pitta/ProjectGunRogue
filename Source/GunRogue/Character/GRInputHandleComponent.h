#pragma once

#include "Components/ActorComponent.h"
#include "GRInputHandleComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUNROGUE_API UGRInputHandleComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGRInputHandleComponent();
	virtual void BeginPlay() override;
};
