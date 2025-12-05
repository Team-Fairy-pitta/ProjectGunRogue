#include "UI/SteamInvite/GRSteamFriendsList.h"

void UGRSteamFriendsList::NativeConstruct()
{
	Super::NativeConstruct();

	UpdateSteamFriends();

	if (!UpdateSteamFriendsTimer.IsValid())
	{
		if (!GetWorld())
		{
			return;
		}
		static float UpdateInterval = 5.0f;
		GetWorld()->GetTimerManager().SetTimer(UpdateSteamFriendsTimer, this, &ThisClass::UpdateSteamFriends, UpdateInterval, true);
	}
}

void UGRSteamFriendsList::NativeDestruct()
{
	Super::NativeDestruct();

	if (UpdateSteamFriendsTimer.IsValid())
	{
		if (!GetWorld())
		{
			return;
		}
		GetWorld()->GetTimerManager().ClearTimer(UpdateSteamFriendsTimer);
		UpdateSteamFriendsTimer.Invalidate();
	}
}
