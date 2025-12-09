// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Controller/GRBossLuwoAIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/TargetPoint.h"

const FName AGRBossLuwoAIController::BossAttackRangeStateKey="BossAttackRangeState";
const FName AGRBossLuwoAIController::FarAttackRandomIndexKey="FarAttackRandomIndex";
const FName AGRBossLuwoAIController::StartJumpTargetPointKey="StartJumpTargetPoint";
const FName AGRBossLuwoAIController::ShieldRegenTargetPointKey="ShieldRegenTargetPoint";
const FName AGRBossLuwoAIController::MapCenterTargetPointKey="MapCenterTargetPoint";

AGRBossLuwoAIController::AGRBossLuwoAIController()
{
}

void AGRBossLuwoAIController::BeginPlay()
{
	Super::BeginPlay();

	if (BlackboardComp)
	{
		InitBlackboardKey();
	}
}


void AGRBossLuwoAIController::InitBlackboardKey()
{
	BlackboardComp->SetValueAsEnum(BossAttackRangeStateKey,static_cast<uint8>(EBossAttackRangeState::None));

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}
	
	TArray<AActor*> FoundActors;
	
	UGameplayStatics::GetAllActorsOfClass(World, ATargetPoint::StaticClass(), FoundActors);
	for (AActor* Actor : FoundActors)
	{
		if (Actor->ActorHasTag(TEXT("StartJump")))
		{
			BlackboardComp->SetValueAsObject(StartJumpTargetPointKey, Actor);
		}
		else if (Actor->ActorHasTag(TEXT("ShieldRegen")))
		{
			BlackboardComp->SetValueAsObject(ShieldRegenTargetPointKey, Actor);
		}
		else if (Actor->ActorHasTag(TEXT("MapCenter")))
		{
			BlackboardComp->SetValueAsObject(MapCenterTargetPointKey, Actor);
		}
	}
}
