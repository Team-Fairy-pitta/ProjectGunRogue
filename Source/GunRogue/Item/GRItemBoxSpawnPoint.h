#pragma once

#include "GameFramework/Actor.h"
#include "GRItemBoxSpawnPoint.generated.h"

class UBoxComponent;
class UArrowComponent;

UCLASS()
class GUNROGUE_API AGRItemBoxSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:
	AGRItemBoxSpawnPoint();
	virtual void BeginPlay() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> BoxComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UArrowComponent> ArrowComponent;
};
