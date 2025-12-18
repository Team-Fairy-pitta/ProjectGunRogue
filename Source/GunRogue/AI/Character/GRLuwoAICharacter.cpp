// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Character/GRLuwoAICharacter.h"
#include "Components/CapsuleComponent.h"

AGRLuwoAICharacter::AGRLuwoAICharacter()
{
	UCapsuleComponent* Capsule = GetCapsuleComponent();
	if (Capsule)
	{
		Capsule->BodyInstance.SetCollisionProfileName(TEXT("GRAIBoss"));
	}

	USkeletalMeshComponent* SkelMesh = GetMesh();
	if (SkelMesh)
	{
		SkelMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1,ECollisionResponse::ECR_Ignore);
	}
}

void AGRLuwoAICharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

void AGRLuwoAICharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	OnLandedEvent.Broadcast();
}
