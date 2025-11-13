#pragma once

#include "GameFramework/Actor.h"
#include "GRNextMapLoader.generated.h"

class UArrowComponent;
class UBoxComponent;

UCLASS()
class GUNROGUE_API AGRNextMapLoader : public AActor
{
	GENERATED_BODY()
	
public:
	AGRNextMapLoader();
	virtual void BeginPlay() override;
	
	void LoadMap(TSoftObjectPtr<UWorld> LevelAsset);

protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> Trigger;

	UPROPERTY(EditDefaultsOnly)
	UArrowComponent* Arrow;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY()
	bool bHasOverlap = false;

	UPROPERTY(EditAnywhere, Category = "Level Streaming")
	TSoftObjectPtr<UWorld> LevelToLoad;
};
