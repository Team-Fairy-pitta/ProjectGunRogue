// Fill out your copyright notice in the Description page of Project Settings.


#include "TestMetaProgression/TestMPPlayerState.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"
#include "AbilitySystem/Attributes/GRHealthAttributeSet.h"
#include "AbilitySystemComponent.h"

ATestMPPlayerState::ATestMPPlayerState()
{
	ASC = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));

	HealthAttributeSet = CreateDefaultSubobject<UGRHealthAttributeSet>(TEXT("HealthAttributeSet"));
	CombatAttributeSet = CreateDefaultSubobject<UGRCombatAttributeSet>(TEXT("CombatAttributeSet"));
}

UAbilitySystemComponent* ATestMPPlayerState::GetAbilitySystemComponent() const
{
	return ASC;
}

void ATestMPPlayerState::BeginPlay()
{
	Super::BeginPlay();

	ASC->InitAbilityActorInfo(this, this);
}
