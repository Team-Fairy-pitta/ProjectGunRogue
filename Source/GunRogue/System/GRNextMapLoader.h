#pragma once

#include "GameFramework/Actor.h"
#include "GRNextMapLoader.generated.h"

UCLASS()
class GUNROGUE_API AGRNextMapLoader : public AActor
{
	GENERATED_BODY()
	
public:
	AGRNextMapLoader();
	virtual void BeginPlay() override;
};
