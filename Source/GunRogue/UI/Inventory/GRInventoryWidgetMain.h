#pragma once

#include "Blueprint/UserWidget.h"
#include "GRInventoryWidgetMain.generated.h"

class UGRInventoryItemWidget;

UCLASS()
class GUNROGUE_API UGRInventoryWidgetMain : public UUserWidget
{
	GENERATED_BODY()
	
public:
	void UpdateInventorySubWidgets();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UGRInventoryItemWidget> InventoryItemWidget;

private:
	void SetWidgetFocusable();
};
