// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Character/GRLuwoAICharacter.h"
#include "AbilitySystemComponent.h"

AGRLuwoAICharacter::AGRLuwoAICharacter()
{
}

void AGRLuwoAICharacter::BeginPlay()
{
	Super::BeginPlay();

	if (ASC && GroundStrikeAbilityClass)
	{
		ASC->InitAbilityActorInfo(this, this);

		FGameplayAbilitySpec Spec(GroundStrikeAbilityClass, 1, /*InputID*/ 0, this);
		ASC->GiveAbility(Spec);
	}
}
