#pragma once

#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/GRAbilitySet.h"
#include "Item/GRItemActor.h"
#include "GRPlayerState.generated.h"

class AGRPlayerController;
class AGRCharacter;
class UGRAbilitySystemComponent;

UCLASS()
class GUNROGUE_API AGRPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AGRPlayerState();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;

	UFUNCTION(BlueprintCallable, Category = "ITPlayerState")
	AGRPlayerController* GetGRPlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "ITPlayerState")
	AGRCharacter* GetGRCharacter() const;

	UFUNCTION(BlueprintCallable, Category = "ITPlayerState")
	UGRAbilitySystemComponent* GetGRAbilitySystemComponent();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(BlueprintCallable)
	bool HasItem(UGRItemDefinition* ItemDefinition);

	UFUNCTION(BlueprintCallable)
	void TryEquipItem(UGRItemDefinition* ItemDefinition, AActor* ItemActor);

	UFUNCTION(BlueprintCallable)
	void UnequipItem(int32 ItemIndex);

	UFUNCTION(BlueprintCallable)
	int32 GetItemNum();

	UFUNCTION(Server, Reliable)
	void ServerRPC_EquipItemActor(UGRItemDefinition* ItemDefinition, AActor* ItemActor);

	UFUNCTION(Server, Reliable)
	void ServerRPC_UnequipItemActor(int32 ItemIndex);

protected:
	UPROPERTY(VisibleAnywhere, Category = "ITPlayerState|AbilitySystemComponent")
	TObjectPtr<UGRAbilitySystemComponent> AbilitySystemComponent;

	FGRAbilitySet_GrantedHandles GrantedHandles;

	UPROPERTY(Replicated)
	TArray<FGRItemHandle> ItemHandles;

	UPROPERTY()
	TSet<UGRItemDefinition*> ItemDefinitionSet;

private:
	UFUNCTION()
	void OnPawnSetted(APlayerState* Player, APawn* NewPawn, APawn* OldPawn);

	void InitAbilitySystemComponent();

	FVector GetGroundPointUsingLineTrace(AActor* SpawnedActor);
	void PlaceActorOnGround(AActor* SpawnedActor);
};
