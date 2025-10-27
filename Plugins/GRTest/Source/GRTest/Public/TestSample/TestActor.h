#pragma once

#include "GameFramework/Actor.h"
#include "TestActor.generated.h"

UCLASS()
class GRTEST_API ATestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ATestActor();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

};
