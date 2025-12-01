#pragma once

#include "GameFramework/Actor.h"
#include "GRItemBoxSpawnManager.generated.h"

class AGRItemBoxSpawnPoint;
class AGRItemRandomBox;

UCLASS()
class GUNROGUE_API AGRItemBoxSpawnManager : public AActor
{
	GENERATED_BODY()
	
public:
	AGRItemBoxSpawnManager();
	virtual void BeginPlay() override;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "GunRogue|Class")
	TSubclassOf<AGRItemRandomBox> ItemRandomBoxClass;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "GunRogue|References")
	TArray<TObjectPtr<AGRItemBoxSpawnPoint>> SpawnPoints;

private:
	void SpawnItemBoxAtRandomLocation();
};
