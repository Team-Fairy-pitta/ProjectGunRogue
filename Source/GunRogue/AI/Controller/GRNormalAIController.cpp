// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Controller/GRNormalAIController.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/Character/GRAICharacter.h"
#include "Character/GRCharacter.h"

const FName AGRNormalAIController::IsPlayerDetectedKey="IsPlayerDetected";
const FName AGRNormalAIController::LastPlayerLocationKey="LastPlayerLocation";
const FName AGRNormalAIController::PatrolStartLocationKey="PatrolStartLocation";

AGRNormalAIController::AGRNormalAIController()
{
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

void AGRNormalAIController::BeginPlay()
{
	Super::BeginPlay();

	if (AIPerceptionComp)
	{
		AIPerceptionComp->OnTargetPerceptionUpdated.AddDynamic(this, &AGRNormalAIController::OnTargetPerceptionUpdated);
	}
}

void AGRNormalAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (BlackboardComp)
	{
		BlackboardComp->SetValueAsVector(PatrolStartLocationKey,InPawn->GetActorLocation());
	}
	
}

void AGRNormalAIController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (AIPerceptionComp)
	{
		AIPerceptionComp->OnTargetPerceptionUpdated.RemoveDynamic(this, &AGRNormalAIController::OnTargetPerceptionUpdated);
	}
	
}

void AGRNormalAIController::OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	////Note : AI끼리 서로 인지될 수 있으므로 예외처리. 추후 삭제.
	AGRAICharacter* AICharacter = Cast<AGRAICharacter>(Actor);
	if (AICharacter)
	{
		return;
	}
	
	//NOTE : 테스트를 위해 ThirdPersonCharacter도 판별을 하기 위해 AGRCharacter에서 ACharacter로 변경.추후 변경.
	//AGRCharacter* NewPlayer = Cast<AGRCharacter>(Actor);
	ACharacter* NewPlayer = Cast<ACharacter>(Actor);
	if (!NewPlayer)
	{
		return;
	}

	if (!BlackboardComp)
	{
		return;
	}
	
	//NOTE : 테스트를 위해 ThirdPersonCharacter도 판별을 하기 위해 AGRCharacter에서 ACharacter로 변경. 추후 변경.
	//AGRCharacter* TargetPlayer=Cast<AGRCharacter>(BlackboardComp->GetValueAsObject(TargetPlayerKey));
	ACharacter* TargetPlayer = Cast<ACharacter>(BlackboardComp->GetValueAsObject(TargetPlayerKey));
	
	if (Stimulus.WasSuccessfullySensed())
	{
		if (!TargetPlayer)
		{
			BlackboardComp->SetValueAsObject(TargetPlayerKey, NewPlayer);
			BlackboardComp->SetValueAsBool(IsPlayerDetectedKey, true);	
		}
	}
	else
	{
		if (TargetPlayer && TargetPlayer==Actor)
		{
			BlackboardComp->SetValueAsVector(LastPlayerLocationKey, TargetPlayer->GetActorLocation());
			UpdateClosestPlayer();	
		}
	}
}

void AGRNormalAIController::UpdateClosestPlayer()
{
	TArray<AActor*> PerceivedActors;
	AIPerceptionComp->GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PerceivedActors);

	float BestDistSq = FLT_MAX;
	AActor* BestActor = nullptr;

	APawn* AIPawn = GetPawn();
	if (!AIPawn)
	{
		return;
	}
	
	for (AActor* PerceivedActor : PerceivedActors)
	{
		////Note : AI끼리 서로 인지될 수 있으므로 예외처리. 추후 삭제.
		AGRAICharacter* AICharacter = Cast<AGRAICharacter>(PerceivedActor);
		if (AICharacter)
		{
			continue;
		}
		
		//NOTE : 테스트를 위해 ThirdPersonCharacter도 판별을 하기 위해 AGRCharacter에서 ACharacter로 변경. 추후 변경
		//AGRCharacter* PerceivedPlayer=Cast<AGRCharacter>(PerceivedActor);
		ACharacter* PerceivedPlayer=Cast<ACharacter>(PerceivedActor);
		if (!PerceivedPlayer)
		{
			continue;
		}

		float DistSq = FVector::DistSquared(AIPawn->GetActorLocation(), PerceivedPlayer->GetActorLocation());
		if (DistSq < BestDistSq)
		{
			BestDistSq = DistSq;
			BestActor = PerceivedPlayer;
		}
	}

	if (!BlackboardComp)
	{
		return;
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
		
		ClearFocus(EAIFocusPriority::Gameplay);
	}
}
