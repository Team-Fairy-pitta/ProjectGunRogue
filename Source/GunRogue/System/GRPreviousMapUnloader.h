#pragma once

#include "GameFramework/Actor.h"
#include "GRPreviousMapUnloader.generated.h"

class UBoxComponent;

UCLASS()
class GUNROGUE_API AGRPreviousMapUnloader : public AActor
{
	GENERATED_BODY()
	
public:
	AGRPreviousMapUnloader();
	virtual void BeginPlay() override;

	void UnloadMap();

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> Trigger;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(VisibleAnywhere)
	bool bHasOverlap = false;
	
	
};
