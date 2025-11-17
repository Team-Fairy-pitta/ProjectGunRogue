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
	
	UFUNCTION()
	void OnLevelLoadCompleted();



protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> Trigger;

	UPROPERTY(EditDefaultsOnly)
	UArrowComponent* Arrow;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY()
	bool bHasOverlap = false;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Level Streaming")
	TSoftObjectPtr<UWorld> LevelToLoad;

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Door")
	class AGRStreamingDoor* TargetDoor;

	void CheckMapLoaderCondition();
	
	UPROPERTY()
	TSet<APlayerState*> PlayersInArea;

	UPROPERTY(ReplicatedUsing = OnRep_IsStream)
	bool bIsStream;

	UFUNCTION()
	void OnRep_IsStream();
	
};
