// GRInvitationSlotWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRInvitationSlotWidget.generated.h"

class UButton;
class UGRBaseTextWidget;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRInvitationSlotWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UButton* InvitationButton;

	UPROPERTY(meta = (BindWidget))
	UGRBaseTextWidget* InvitationText;
	
	virtual void NativeConstruct() override;

public:
	void SetInvitationText(const FText& InText);

private:
	UFUNCTION()
	void OnInvitationClicked();
};
