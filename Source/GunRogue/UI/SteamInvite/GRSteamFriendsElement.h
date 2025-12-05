#pragma once

#include "Blueprint/UserWidget.h"
#include "GRSteamFriendsElement.generated.h"

class UImage;
class UTextBlock;
class UButton;

UCLASS()
class GUNROGUE_API UGRSteamFriendsElement : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> AvatarImage;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> PlayerNameText;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> InviteButton;
};
