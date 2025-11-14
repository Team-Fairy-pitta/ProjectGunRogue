// GRInvitationHUDWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRInvitationHUDWidget.generated.h"

class UGRInvitationListWidget;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRInvitationHUDWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UGRInvitationListWidget* GetInvitationListWidget() const { return InvitationList; }
	
protected:
	UPROPERTY(meta = (BindWidget))
	UGRInvitationListWidget* InvitationList;
};
