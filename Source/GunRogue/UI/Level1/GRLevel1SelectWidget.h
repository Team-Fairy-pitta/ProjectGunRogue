#pragma once

#include "Blueprint/UserWidget.h"
#include "GRLevel1SelectWidget.generated.h"

UCLASS()
class GUNROGUE_API UGRLevel1SelectWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:
	void SetWidgetFocusable();
};
