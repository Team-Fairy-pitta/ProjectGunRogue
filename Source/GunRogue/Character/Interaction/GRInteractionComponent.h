#pragma once

#include "Components/ActorComponent.h"
#include "GRInteractionComponent.generated.h"

class UBoxComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUNROGUE_API UGRInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGRInteractionComponent();
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);
	AActor* TraceForInteractable();

protected:
	void FindInteractableActor();
	AActor* DoLineTrace(FVector& StartLocation, FVector& EndLocation);

	void OnOver(AActor* InActor);
	void OnOut(AActor* InActor);

	void AddOutline(AActor* InActor);
	void RemoveOutline(AActor* InActor);

	UPROPERTY()
	AActor* FocusedActor;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Interaction|Configuration")
	float InteractionDistance = 650.0f;
};
