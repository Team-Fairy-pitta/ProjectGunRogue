// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GRBossLuwoAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

const FName AGRBossLuwoAIController::TargetPlayerKey="TargetPlayer";
const FName AGRBossLuwoAIController::LastPlayerLocationKey="LastPlayerLocation";
const FName AGRBossLuwoAIController::BossAttackRangeStateKey="BossAttackRangeState";
const FName AGRBossLuwoAIController::FarAttackRandomIndexKey="FarAttackRandomIndex";

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

	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsEnum(BossAttackRangeStateKey,static_cast<uint8>(EBossAttackRangeState::None));
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
