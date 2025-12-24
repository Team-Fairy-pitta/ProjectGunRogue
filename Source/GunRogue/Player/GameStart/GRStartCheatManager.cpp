#include "Player/GameStart/GRStartCheatManager.h"
#include "Kismet/GameplayStatics.h"

void UGRStartCheatManager::ConnectToServer(const FString& IPAddress)
{
	if (APlayerController* PC = GetPlayerController())
	{
		FString TravelURL = IPAddress;
		PC->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
	}
}

void UGRStartCheatManager::ConnectToServerWithPort(const FString& IPAddress, int32 Port)
{
	if (APlayerController* PC = GetPlayerController())
	{
		FString TravelURL = FString::Printf(TEXT("%s:%d"), *IPAddress, Port);
		PC->ClientTravel(TravelURL, ETravelType::TRAVEL_Absolute);
	}
}
