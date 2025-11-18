// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GRStreamingDoorController.generated.h"

class UBoxComponent;
class AGRStreamingDoor;

UCLASS()
class GUNROGUE_API AGRStreamingDoorController : public AActor
{
	GENERATED_BODY()
	
public:	

	AGRStreamingDoorController();

	UFUNCTION()
	void SetLoadLevelCompleted();

protected:

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UBoxComponent* TriggerBox;

	UPROPERTY()
	TSet<APlayerState*> PlayersInArea;

	UPROPERTY(ReplicatedUsing = OnRep_IsDoorOpen)
	bool bIsDoorOpen;

	bool bHasLevelCompleted;

	UFUNCTION()
	void OnRep_IsDoorOpen();
	

	void CheckDoorOpenCondition();

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "GR")
	TObjectPtr<AGRStreamingDoor> TargetDoor;

	UPROPERTY()
	bool bWasActivated = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GR")
	bool bRequiresLevelCompletion = false;

private:

	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);


	
};
