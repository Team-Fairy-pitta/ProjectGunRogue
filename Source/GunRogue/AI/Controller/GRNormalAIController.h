// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AI/Controller/GRAIController.h"
#include "GRNormalAIController.generated.h"

/**
 * 
 */

struct FAIStimulus;
 class UAIPerceptionComponent;
 class UAISenseConfig_Sight;
 
UCLASS()
class GUNROGUE_API AGRNormalAIController : public AGRAIController
{
	GENERATED_BODY()
	
public:
	AGRNormalAIController();
	
protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	UFUNCTION()
	void OnTargetPerceptionUpdated(AActor* Actor, FAIStimulus Stimulus);

	void UpdateClosestPlayer();

public:
	static const FName IsPlayerDetectedKey;
	static const FName LastPlayerLocationKey;
	static const FName PatrolStartLocationKey;
	
private:
	UPROPERTY(VisibleAnywhere, Category = "AI")
	TObjectPtr<UAIPerceptionComponent> AIPerceptionComp;
	
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	TObjectPtr<UAISenseConfig_Sight> SightConfig;
};
