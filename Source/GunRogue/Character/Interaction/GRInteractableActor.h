#pragma once

#include "CoreMinimal.h"
#include "GRInteractableActor.generated.h"

UINTERFACE(MinimalAPI, NotBlueprintable)
class UGRInteractableActor : public UInterface
{
	GENERATED_BODY()
};

class IGRInteractableActor
{
	GENERATED_BODY()

public:
	virtual TArray<TObjectPtr<UStaticMeshComponent>> GetMeshComponents() = 0;
	virtual void InteractWith(AActor* OtherActor) = 0;
};
