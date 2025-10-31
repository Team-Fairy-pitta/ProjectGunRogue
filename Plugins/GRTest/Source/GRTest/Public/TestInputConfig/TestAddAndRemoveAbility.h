#pragma once

#include "GameFramework/Actor.h"
#include "AbilitySystem/GRAbilitySet.h"
#include "TestAddAndRemoveAbility.generated.h"

class UBoxComponent;
class UStaticMeshComponent;
class UGRAbilitySet;

UCLASS()
class GRTEST_API ATestAddAndRemoveAbility : public AActor
{
	GENERATED_BODY()

public:
	ATestAddAndRemoveAbility();
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnAdd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnRemove(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AbilitySet")
	TObjectPtr<UGRAbilitySet> AbilitySet;

	FGRAbilitySet_GrantedHandles Handles;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> Add;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UStaticMeshComponent> Remove;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> AddBox;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UBoxComponent> RemoveBox;

	bool bIsAdded;
};
