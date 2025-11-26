#pragma once

#include "GameFramework/Actor.h"
#include "Character/Interaction/GRInteractableActor.h"
#include "GameModes/Level1/GRLevel1Data.h"
#include "GRLevel1ControlPanel.generated.h"

class UStaticMeshComponent;
class UWidgetComponent;
class UUserWidget;
class AGRStreamingDoor;
class AGRNextMapLoader;

UCLASS()
class GUNROGUE_API AGRLevel1ControlPanel : public AActor, public IGRInteractableActor
{
	GENERATED_BODY()

public:
	AGRLevel1ControlPanel();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// IGRInteractableActor
	virtual TArray<TObjectPtr<UStaticMeshComponent>> GetMeshComponents() override;
	virtual void InteractWith(AActor* OtherActor) override;
	virtual void OnOver() override;
	virtual void OnOut() override;
	virtual bool CanInteract(AActor* OtherActor) override;

	void OnUsePanel(FGRLevel1Node NextNode);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent_Sub1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent_Sub2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent_Sub3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UWidgetComponent> InteractWidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GunRogue|WidgetClass")
	TSubclassOf<UUserWidget> InteractWidgetClass;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "GunRogue|TargetDoor")
	TObjectPtr<AGRStreamingDoor> TargetDoorInstance;

	UPROPERTY(EditInstanceOnly, BlueprintReadOnly, Category = "GunRogue|TargetMapLoader")
	TObjectPtr<AGRNextMapLoader> MapLoaderInstance;

	UPROPERTY(Replicated)
	int8 bWasActivated;

	UPROPERTY(ReplicatedUsing = OnRep_bIsDoorOpen)
	int8 bIsDoorOpen;

	UPROPERTY(ReplicatedUsing = OnRep_CachedNextNode)
	FGRLevel1Node CachedNextNode;

	UFUNCTION()
	void OnRep_bIsDoorOpen();

	UFUNCTION()
	void OnRep_CachedNextNode();
};
