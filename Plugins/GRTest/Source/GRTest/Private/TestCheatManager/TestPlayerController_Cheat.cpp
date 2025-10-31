// Fill out your copyright notice in the Description page of Project Settings.


#include "TestCheatManager/TestPlayerController_Cheat.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "CheatManager/GRCheatManager.h"

ATestPlayerController_Cheat::ATestPlayerController_Cheat()
{
	CheatClass = UGRCheatManager::StaticClass();
}
