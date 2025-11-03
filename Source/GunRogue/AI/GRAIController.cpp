// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GRAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"

AGRAIController::AGRAIController()
	:BehaviorTreeAsset(nullptr)
	,BlackboardAsset(nullptr)
	,BehaviorComp(nullptr)
	,BlackboardComp(nullptr)
{
	BehaviorComp = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviorComp"));
	BlackboardComp = CreateDefaultSubobject<UBlackboardComponent>(TEXT("BlackboardComp"));
	
	AIPerceptionComp = CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("AIPerceptionComp"));
	
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));
	SightConfig->SightRadius = 2000.f;
	SightConfig->LoseSightRadius = 2200.f;
	SightConfig->PeripheralVisionAngleDegrees = 60.f;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = false;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = false;

	AIPerceptionComp->ConfigureSense(*SightConfig);
}

void AGRAIController::BeginPlay()
{
	Super::BeginPlay();
	
	if (AIPerceptionComp)
	{
		AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AGRAIController::OnTargetPerceptionUpdated);
	}
	
}

void AGRAIController::OnPossess(APawn* InPawn)
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

void AGRAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{

}
