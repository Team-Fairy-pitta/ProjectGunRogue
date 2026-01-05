// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Robot/Drone/GRDroneManagerComponent.h"
#include "AI/Robot/Drone/GRDrone.h"
#include "Net/UnrealNetwork.h"

UGRDroneManagerComponent::UGRDroneManagerComponent()
	:MaxDroneNum(5)
	,DroneSpawnOffset(FVector(100.0f,0.0f,100.0f))
	,DroneClass(nullptr)
{
	PrimaryComponentTick.bCanEverTick = false;

	SetIsReplicatedByDefault(true);
}

void UGRDroneManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, SpawnedDrones);
}

void UGRDroneManagerComponent::SpawnDrone()
{
	AActor* Player = GetOwner();
	if (!Player)
	{
		return;
	}
	
	if (!Player->HasAuthority())
	{
		return;
	}
	
	if (!DroneClass)
	{
		return;
	}

	if (SpawnedDrones.Num() >= MaxDroneNum)
	{
		return;
	}
	
	FVector SpawnLoc =  Player-> GetActorLocation() + DroneSpawnOffset;
	FRotator SpawnRot = Player->GetActorRotation();

	FActorSpawnParameters Params;
	Params.Owner = Player;
	Params.Instigator = Cast<APawn>(Player);
	Params.SpawnCollisionHandlingOverride =
	ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	
	AGRDrone* NewDrone = GetWorld()->SpawnActor<AGRDrone>(DroneClass, SpawnLoc, SpawnRot, Params);
	if (NewDrone)
	{
		SpawnedDrones.Add(NewDrone);
		UpdateDroneFormation();
	}

}

void UGRDroneManagerComponent::UpdateDroneFormation()
{
	AActor* Player = GetOwner();
	int32 TotalDroneNum = SpawnedDrones.Num();
	
	for (int i =0 ;i < TotalDroneNum; i++)
	{
		AGRDrone* SpawnedDrone = Cast<AGRDrone>(SpawnedDrones[i]);
		if (!SpawnedDrone)
		{
			continue;
		}

		SpawnedDrone->SetTargetOffset(Player,i,TotalDroneNum);
	}
}

void UGRDroneManagerComponent::RemoveDrone(AGRDrone* Drone)
{
	if (!Drone)
	{
		return;
	}

	SpawnedDrones.Remove(Drone);

	UpdateDroneFormation();
}

void UGRDroneManagerComponent::CommandAllDronesAttack(const FVector AimDir)
{
	for (AGRDrone* Drone : SpawnedDrones)
	{
		if (!IsValid(Drone))
		{
			continue;
		}

		Drone->StartMainSkillAttack(AimDir);
	}

	SpawnedDrones.Empty();
}
