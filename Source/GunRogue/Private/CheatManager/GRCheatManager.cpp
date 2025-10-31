// Fill out your copyright notice in the Description page of Project Settings.


#include "CheatManager/GRCheatManager.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "TestCheatManager/TestGRCharacter_Cheat.h"


void UGRCheatManager::Flying()
{
	ACharacter* MyCharacter = Cast<ACharacter>(GetOuterAPlayerController()->GetPawn());
	if (!MyCharacter) return;

	UCharacterMovementComponent* MoveComp = MyCharacter->GetCharacterMovement();
	if (!MoveComp) return;

	if (MoveComp->MovementMode != MOVE_Flying)
	{
		MoveComp->SetMovementMode(MOVE_Flying);
		MoveComp->GravityScale = 0.0f;
		MoveComp->BrakingFrictionFactor = 0.f; // 관성 줄이기 (자유비행 느낌)
		UE_LOG(LogTemp, Log, TEXT("비행 모드"));	
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("현재 비행 모드"));
	}
}

void UGRCheatManager::Walking()
{
	ACharacter* MyCharacter = Cast<ACharacter>(GetOuterAPlayerController()->GetPawn());
	if (!MyCharacter) return;

	UCharacterMovementComponent* MoveComp = MyCharacter->GetCharacterMovement();
	if (!MoveComp) return;

	if (MoveComp->MovementMode != MOVE_Walking)
	{
		MoveComp->SetMovementMode(MOVE_Walking);
		MoveComp->GravityScale = 1.0f;
		UE_LOG(LogTemp, Log, TEXT("걷기 모드"));
	}
	{
		UE_LOG(LogTemp, Log, TEXT("현재 걷기 모드"));
	}
}


