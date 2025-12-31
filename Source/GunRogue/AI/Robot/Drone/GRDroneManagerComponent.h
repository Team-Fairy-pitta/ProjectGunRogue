// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GRDroneManagerComponent.generated.h"


class AGRDrone;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUNROGUE_API UGRDroneManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGRDroneManagerComponent();
	const TArray<AGRDrone*>& GetDrones() const { return SpawnedDrones; }
	void SpawnDrone();
	void RemoveDrone(AGRDrone* Drone);
	void CommandAllDronesAttack(const FVector AimDir);

protected:
	void UpdateDroneFormation();
	
protected:
	UPROPERTY()
	TArray<AGRDrone*> SpawnedDrones;

	UPROPERTY(EditAnywhere,Category="Drone")
	int32 MaxDroneNum;

	UPROPERTY(EditAnywhere,Category="Drone")
	FVector DroneSpawnOffset;

	UPROPERTY(EditAnywhere,Category="Drone")
	TSubclassOf<AGRDrone> DroneClass;

};
