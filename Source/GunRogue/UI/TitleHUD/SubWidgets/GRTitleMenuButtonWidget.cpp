// GRTitleMenuButtonWidget.cpp


#include "UI/TitleHUD/SubWidgets/GRTitleMenuButtonWidget.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"

void UGRTitleMenuButtonWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (MainButtonText)
	{
		MainButtonText->SetText(DefaultText);
	}
}

void UGRTitleMenuButtonWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (MainButton)
	{
		MainButton->OnClicked.AddDynamic(this, &UGRTitleMenuButtonWidget::OnMainClicked);
	}
}

void UGRTitleMenuButtonWidget::OnMainClicked()
{
	OnTitleButtonClicked.Broadcast();
}
