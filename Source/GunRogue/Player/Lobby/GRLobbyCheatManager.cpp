#include "Player/Lobby/GRLobbyCheatManager.h"
#include "Player/Lobby/GRLobbyPlayerState.h"

void UGRLobbyCheatManager::SetMetaGoods(int32 InValue)
{
	APlayerController* PlayerController = GetPlayerController();
	if (!IsValid(PlayerController))
	{
		return;
	}

	AGRLobbyPlayerState* LobbyPlayerState = PlayerController->GetPlayerState<AGRLobbyPlayerState>();
	if (!IsValid(LobbyPlayerState))
	{
		return;
	}

	LobbyPlayerState->SetMetaGoods(InValue);
}

void UGRLobbyCheatManager::ShowNetworkInfo()
{
	if (UWorld* World = GetWorld())
	{
		UNetDriver* NetDriver = World->GetNetDriver();

		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Green, TEXT("= Network Information ="));
		}

		if (NetDriver)
		{
			int32 Port = NetDriver->GetLocalAddr()->GetPort();
			FString IPAddress = NetDriver->GetLocalAddr()->ToString(false);

			FString NetModeStr;
			ENetMode NetMode = World->GetNetMode();
			switch (NetMode)
			{
			case NM_DedicatedServer:
				NetModeStr = TEXT("Dedicated Server");
				break;
			case NM_ListenServer:
				NetModeStr = TEXT("Listen Server");
				break;
			case NM_Client:
				NetModeStr = TEXT("Client");
				break;
			default:
				NetModeStr = TEXT("Standalone");
				break;
			}

			int32 ClientCount = NetDriver->ClientConnections.Num();
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString::Printf(TEXT("IP: %s"), *IPAddress));
				GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Cyan, FString::Printf(TEXT("Port: %d"), Port));
				GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Yellow, FString::Printf(TEXT("Mode: %s"), *NetModeStr));
				GEngine->AddOnScreenDebugMessage(-1, 15.f, FColor::Magenta, FString::Printf(TEXT("Connected Clients: %d"), ClientCount));
			}
		}
		else
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("No network driver found!"));
			}
		}
	}
}
