// Fill out your copyright notice in the Description page of Project Settings.


#include "System/GRStreamingDoor.h"


AGRStreamingDoor::AGRStreamingDoor()
{
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(RootComponent);

	bReplicates = true;
	
}

void AGRStreamingDoor::BeginPlay()
{
	Super::BeginPlay();
}





