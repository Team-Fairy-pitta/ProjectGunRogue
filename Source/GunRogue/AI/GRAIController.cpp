// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GRAIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "Character/GRCharacter.h"

const FName AGRAIController::TargetPlayerKey="TargetPlayer";
const FName AGRAIController::IsPlayerDetectedKey="IsPlayerDetected";

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
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;

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
	AGRCharacter* TargetPlayer=Cast<AGRCharacter>(BlackboardComp->GetValueAsObject(TargetPlayerKey));
	
	if (Stimulus.WasSuccessfullySensed())
	{
		if (TargetPlayer)	return;

		AGRCharacter* NewPlayer=Cast<AGRCharacter>(Actor);
		if (NewPlayer)
		{
			BlackboardComp->SetValueAsObject(TargetPlayerKey, NewPlayer);
			BlackboardComp->SetValueAsBool(IsPlayerDetectedKey, true);
		}
	}
	else
	{
		if (TargetPlayer==Actor)
		{
			UpdateClosestPlayer();	
		}
	}
}

void AGRAIController::UpdateClosestPlayer()
{
	TArray<AActor*> PerceivedActors;
	AIPerceptionComp->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActors);

	float BestDistSq = FLT_MAX;
	AActor* BestActor = nullptr;

	APawn* MyPawn = GetPawn();
	if (!MyPawn) return;
	
	for (AActor* PerceivedActor : PerceivedActors)
	{
		AGRCharacter* PerceivedPlayer=Cast<AGRCharacter>(PerceivedActor);
		if (!PerceivedPlayer)	continue;

		float DistSq = FVector::DistSquared(MyPawn->GetActorLocation(), PerceivedPlayer->GetActorLocation());
		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			BestActor = PerceivedPlayer;
		}
	}

	if (IsValid(BestActor))
	{
		BlackboardComp->SetValueAsObject(TargetPlayerKey, BestActor);
		BlackboardComp->SetValueAsBool(IsPlayerDetectedKey, true);
	}
	else
	{
		BlackboardComp->SetValueAsObject(TargetPlayerKey, nullptr);
		BlackboardComp->SetValueAsBool(IsPlayerDetectedKey, false);
	}
}
