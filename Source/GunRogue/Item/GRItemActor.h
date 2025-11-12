#pragma once

#include "GameFramework/Actor.h"
#include "Character/Interaction/GRInteractableActor.h"
#include "GRItemActor.generated.h"

// Item Actor
// 바닥(=맵, 레벨)에 떨어져 있는 아이템을 나타내기 위한 Actor입니다.

class UBoxComponent;
class UStaticMeshComponent;

UCLASS()
class GUNROGUE_API AGRItemActor : public AActor, public IGRInteractableActor
{
	GENERATED_BODY()
	
public:
	AGRItemActor();
	virtual void BeginPlay() override;

	// IGRInteractableActor
	virtual TArray<TObjectPtr<UStaticMeshComponent>> GetMeshComponents() override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UBoxComponent> InteractionBoxComponent;
};
