// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GRBossLuwoAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/TargetPoint.h"

const FName AGRBossLuwoAIController::TargetPlayerKey="TargetPlayer";
const FName AGRBossLuwoAIController::LastPlayerLocationKey="LastPlayerLocation";
const FName AGRBossLuwoAIController::BossAttackRangeStateKey="BossAttackRangeState";
const FName AGRBossLuwoAIController::FarAttackRandomIndexKey="FarAttackRandomIndex";
const FName AGRBossLuwoAIController::StartJumpTargetPointKey="StartJumpTargetPoint";
const FName AGRBossLuwoAIController::ShieldRegenTargetPointKey="ShieldRegenTargetPoint";
const FName AGRBossLuwoAIController::MapCenterTargetPointKey="MapCenterTargetPoint";

AGRBossLuwoAIController::AGRBossLuwoAIController()
	:BehaviorTreeAsset(nullptr)
	,BlackboardAsset(nullptr)
	,BehaviorComp(nullptr)
	,BlackboardComp(nullptr)
{
	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
}

void AGRBossLuwoAIController::BeginPlay()
{
	Super::BeginPlay();

	if (BlackboardComp)
	{
		InitBlackboardKey();
	}
}

void AGRBossLuwoAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (BehaviorTreeAsset && BlackboardAsset)
	{
		if (UseBlackboard(BlackboardAsset, BlackboardComp))
		{
			BlackboardComp->InitializeBlackboard(*BlackboardAsset);
		}
		RunBehaviorTree(BehaviorTreeAsset);
	}
}

void AGRBossLuwoAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (BehaviorComp)
	{
		BehaviorComp->StopTree(EBTStopMode::Safe);
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
