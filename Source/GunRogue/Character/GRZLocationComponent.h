#pragma once

#include "Components/ActorComponent.h"
#include "GRZLocationComponent.generated.h"

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUNROGUE_API UGRZLocationComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGRZLocationComponent();
	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WorldBorderMin = -10000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float WorldBorderMax = 10000;

private:
	bool CheckOutOfWorld();
	void SetHealthZero();
};
