// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Character/GRLuwoAICharacter.h"
#include "AbilitySystemComponent.h"

AGRLuwoAICharacter::AGRLuwoAICharacter()
{
}

void AGRLuwoAICharacter::BeginPlay()
{
	Super::BeginPlay();

	if (ASC)
	{
		ASC->InitAbilityActorInfo(this, this);
		
		for (auto& AbilityClass : AttackAbilityClassList)
		{
			if (AbilityClass)
			{
				FGameplayAbilitySpec Spec(AbilityClass, /*Level*/1, /*InputID*/0, this);
				ASC->GiveAbility(Spec);
			}
		}
	}
}

void AGRLuwoAICharacter::Landed(const FHitResult& Hit)
{
	Super::Landed(Hit);

	OnLandedEvent.Broadcast();
}
