#pragma once

#include "Blueprint/UserWidget.h"
#include "GRSteamFriendsList.generated.h"

class UVerticalBox;

UCLASS()
class GUNROGUE_API UGRSteamFriendsList : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UVerticalBox> FriendList;

	FTimerHandle UpdateSteamFriendsTimer;

	UFUNCTION(BlueprintImplementableEvent)
	void UpdateSteamFriends();
};
