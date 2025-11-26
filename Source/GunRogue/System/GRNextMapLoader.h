#pragma once

#include "GameFramework/Actor.h"
#include "GRNextMapLoader.generated.h"

class AGRStreamingDoorController;
class UArrowComponent;
class UBoxComponent;
class AGRStreamingDoor;

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

	void SetLevelToLoad(TSoftObjectPtr<UWorld> InLevelToLoad);


protected:
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UBoxComponent> Trigger;

	UPROPERTY(EditDefaultsOnly)
	UArrowComponent* Arrow;
	
	UPROPERTY()
	bool bWasActivated = false;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GRLoader")
	TSoftObjectPtr<UWorld> LevelToLoad;
	
	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "GRLoader")
	TObjectPtr<AGRStreamingDoorController> TargetController;

	void CheckMapLoaderCondition();
	
	UPROPERTY()
	TSet<APlayerState*> PlayersInArea;

	UPROPERTY(ReplicatedUsing = OnRep_ShouldLoadLevel)
	bool bShouldLoadLevel;

	UFUNCTION()
	void OnRep_ShouldLoadLevel();

private:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	
};
