// GRInvitationListWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRInvitationListWidget.generated.h"

class UGRInvitationSlotWidget;
class UScrollBox;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRInvitationListWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UScrollBox* InvitationContainer;

	UPROPERTY(EditDefaultsOnly, Category="Invitation")
	TSubclassOf<UGRInvitationSlotWidget> InvitationSlotClass;

	UPROPERTY()
	TArray<UGRInvitationSlotWidget*> InvitationSlots;

public:
	UFUNCTION(BlueprintCallable)
	void CreateInvitationSlot();
};
