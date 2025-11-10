#pragma once

#include "GameFramework/Actor.h"
#include "GRPreviousMapUnloader.generated.h"

UCLASS()
class GUNROGUE_API AGRPreviousMapUnloader : public AActor
{
	GENERATED_BODY()
	
public:
	AGRPreviousMapUnloader();
	virtual void BeginPlay() override;
};
