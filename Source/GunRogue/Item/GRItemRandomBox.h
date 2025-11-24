#pragma once

#include "GameFramework/Actor.h"
#include "Character/Interaction/GRInteractableActor.h"
#include "GRItemRandomBox.generated.h"

class UGRItemDefinition;
class AGRPlayerState;

USTRUCT()
struct FSpawnedActor
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> Actors;
};

UCLASS()
class GUNROGUE_API AGRItemRandomBox : public AActor, public IGRInteractableActor
{
	GENERATED_BODY()
	
public:
	AGRItemRandomBox();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	// IGRInteractableActor
	virtual TArray<TObjectPtr<UStaticMeshComponent>> GetMeshComponents() override;
	virtual void InteractWith(AActor* OtherActor) override;
	virtual void OnOver() override;
	virtual void OnOut() override;
	virtual bool CanInteract(AActor* OtherActor) override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_StartAnimation(AGRPlayerState* GRPlayerState);

	UFUNCTION(BlueprintImplementableEvent)
	void StartOpenAnimation(AGRPlayerState* GRPlayerState);

	UFUNCTION(BlueprintCallable)
	void OnFinishOpenAnimation(AGRPlayerState* GRPlayerState);

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Configuration")
	float RarityProbatility_Normal = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Configuration")
	float RarityProbatility_Rare = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item|Configuration")
	float RarityProbatility_Epic = 0.0f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BoxBottom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> BoxLid;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Item|Table")
	TObjectPtr<UDataTable> ItemTable;

private:
	TArray<UGRItemDefinition*> GetNewRandomItems(AGRPlayerState* GRPlayerState);
	UGRItemDefinition* GetNewRandomItem(AGRPlayerState* GRPlayerState, TArray<UGRItemDefinition*>& AlreadySelected);

	void SpawnItemsToSpecificPlayer(AGRPlayerState* GRPlayerState, TArray<UGRItemDefinition*> ItemDefinitions);
	void SpawnItemToSpecificPlayer(AGRPlayerState* GRPlayerState, UGRItemDefinition* ItemDefinition, FVector& Location);

	void DisableToSpecificPlayer(AGRPlayerState* GRPlayerState);

	void OnPickupAnyItem(AGRPlayerState* GRPlayerState);

	UPROPERTY(Replicated)
	TArray<AGRPlayerState*> WasActivatedArray;

	UPROPERTY()
	TMap<AGRPlayerState*, FSpawnedActor> SpawnedActors;
};
