// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Character/GRLuwoAICharacter.h"
#include "Components/CapsuleComponent.h"

#include "GameModes/Level1/GRGameState_Level1.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Attributes/GRHealthAttributeSet.h"


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

	if (ASC)
	{
		ASC->GetGameplayAttributeValueChangeDelegate(UGRHealthAttributeSet::GetHealthAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnBossHealthChanged.Broadcast(Data.NewValue);
		});

		ASC->GetGameplayAttributeValueChangeDelegate(UGRHealthAttributeSet::GetShieldAttribute()).AddLambda([this](const FOnAttributeChangeData& Data)
		{
			OnBossShieldChanged.Broadcast(Data.NewValue);
		});
	}

	if (HasAuthority())
	{
		if (AGRGameState_Level1* GS = GetWorld()->GetGameState<AGRGameState_Level1>())
		{
			GS->SetCurrentBoss(this);
		}
	}
	
}

void AGRLuwoAICharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	OnLandedEvent.Broadcast();
}

void AGRLuwoAICharacter::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (HasAuthority())
	{
		if (AGRGameState_Level1* GS = GetWorld()->GetGameState<AGRGameState_Level1>())
		{
			if (GS->GetCurrentBoss() == this)
			{
				GS->ClearCurrentBoss();
			}
		}
	}
}

float AGRLuwoAICharacter::GetBossHealth() const
{
	return HealthAttributeSet->GetHealth();
}

float AGRLuwoAICharacter::GetBossMaxHealth() const
{
	return HealthAttributeSet->GetMaxHealth();
}

float AGRLuwoAICharacter::GetBossShield() const
{
	return HealthAttributeSet->GetShield();
}

float AGRLuwoAICharacter::GetBossMaxShield() const
{
	return HealthAttributeSet->GetMaxShield();
}
