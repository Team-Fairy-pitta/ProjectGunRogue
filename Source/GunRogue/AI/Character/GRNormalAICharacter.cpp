// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Character/GRNormalAICharacter.h"
#include "Components/CapsuleComponent.h"

AGRNormalAICharacter::AGRNormalAICharacter()
{
	UCapsuleComponent* Capsule = GetCapsuleComponent();
	if (Capsule)
	{
		Capsule->BodyInstance.SetCollisionProfileName(TEXT("AIBoss"));
	}

	USkeletalMeshComponent* SkelMesh = GetMesh();
	if (SkelMesh)
	{
		SkelMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel1,ECollisionResponse::ECR_Ignore);
	}
}
