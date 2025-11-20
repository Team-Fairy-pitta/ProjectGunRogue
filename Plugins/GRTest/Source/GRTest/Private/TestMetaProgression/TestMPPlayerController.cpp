// Fill out your copyright notice in the Description page of Project Settings.


#include "TestMetaProgression/TestMPPlayerController.h"
#include "GameFramework/PlayerState.h"
#include "MetaProgression/GRPerkSubsystem.h"

void ATestMPPlayerController::RequestLocalSave()
{
	if (!IsLocalController())
	{
		return;
	}

	UGRPerkSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UGRPerkSubsystem>();
	if (!Subsystem)
	{
		return;
	}
	
	FString PlayerId = GetUniquePlayerId();
	Subsystem->SetLocalPlayerUniqueId(PlayerId);

	Subsystem->SavePerks();

	UE_LOG(LogTemp, Warning, TEXT("[Save] MetaGoods: %d"), Subsystem->GetMetaGoods());
}

void ATestMPPlayerController::RequestLocalLoad()
{
	if (!IsLocalController())
	{
		return;
	}

	UGRPerkSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UGRPerkSubsystem>();
	if (!Subsystem)
	{
		return;
	}

	FString PlayerId = GetUniquePlayerId();
	Subsystem->SetLocalPlayerUniqueId(PlayerId);

	Subsystem->LoadPerks();

	UE_LOG(LogTemp, Warning, TEXT("[Load] MetaGoods: %d"), Subsystem->GetMetaGoods());
}

void ATestMPPlayerController::AddMetaGoodsToTest()
{
	UGRPerkSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UGRPerkSubsystem>();
	if (!Subsystem)
	{
		return;
	}
	
	FString PlayerId = GetUniquePlayerId();
	Subsystem->SetLocalPlayerUniqueId(PlayerId);

	Subsystem->AddMetaGoods(1000);

	UE_LOG(LogTemp, Warning, TEXT("MetaGoods Added"));
}

void ATestMPPlayerController::PrintPerkInfoRows()
{
	UGRPerkSubsystem* Subsystem = GetGameInstance()->GetSubsystem<UGRPerkSubsystem>();
	if (!Subsystem)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("------ Current Perk Levels ------"));
	
	if (!PerkTable)
	{
		UE_LOG(LogTemp, Error, TEXT("PerkTable is NULL!"));
	}

	TArray<FName> RowNames = PerkTable->GetRowNames();

	for (const FName& PerkName : RowNames)
	{
		int32 Level = Subsystem->GetPerkInfoRow(PerkName);

		UE_LOG(LogTemp, Warning, TEXT("Perk: %s / Level: %d"),
			*PerkName.ToString(),
			Level);
	}
}

FString ATestMPPlayerController::GetUniquePlayerId() const
{
	if (PlayerState)
	{
		return FString::Printf(TEXT("NetID_%d"), PlayerState->GetPlayerId());
	}

	if (ULocalPlayer* LocalPlayer = GetLocalPlayer())
	{
		return FString::Printf(TEXT("ControllerID_Fallback_%d"),LocalPlayer->GetControllerId());
	}

	return TEXT("UnKnown_Error_ID");
}
