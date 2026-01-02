#pragma once

#include "GameFramework/Actor.h"
#include "Character/Interaction/GRInteractableActor.h"
#include "GRAugmentSelector.generated.h"

class UStaticMeshComponent;
class UUserWidget;
class UWidgetComponent;
class AGRPlayerState;

UCLASS()
class GUNROGUE_API AGRAugmentSelector : public AActor, public IGRInteractableActor
{
	GENERATED_BODY()
	
public:	
	AGRAugmentSelector();
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	// IGRInteractableActor
	virtual TArray<TObjectPtr<UStaticMeshComponent>> GetMeshComponents() override;
	virtual void InteractWith(AActor* OtherActor) override;
	virtual void OnOver() override;
	virtual void OnOut() override;
	virtual bool CanInteract(AActor* OtherActor) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UWidgetComponent> InteractWidgetComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GunRogue|WidgetClass")
	TSubclassOf<UUserWidget> InteractWidgetClass;

private:
	UPROPERTY(Replicated)
	TArray<AGRPlayerState*> WasActivatedArray;

	void DisableToSpecificPlayer(AGRPlayerState* GRPlayerState);
};
