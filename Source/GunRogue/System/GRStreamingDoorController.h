// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GRStreamingDoorController.generated.h"

class UBoxComponent;

UCLASS()
class GUNROGUE_API AGRStreamingDoorController : public AActor
{
	GENERATED_BODY()
	
public:	

	AGRStreamingDoorController();

protected:

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY(VisibleAnywhere,BlueprintReadOnly)
	UBoxComponent* TriggerBox;

	UPROPERTY()
	TSet<APlayerState*> PlayersInArea;

	UPROPERTY(ReplicatedUsing = OnRep_IsDoorOpen)
	bool bIsDoorOpen;

	UFUNCTION()
	void OnRep_IsDoorOpen();

	void CheckDoorOpenCondition();

	UPROPERTY(EditInstanceOnly, BlueprintReadWrite, Category = "Door")
	class AGRStreamingDoor* TargetDoor;

private:

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
};
