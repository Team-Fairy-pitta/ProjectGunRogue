#include "Item/GRItemInfoWidget.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"

void UGRItemInfoWidget::InitItemInfo(UTexture2D* Icon, const FText& Name, const FText& Description)
{
	if (ItemNameText)
	{
		ItemNameText->SetText(Name);
	}

	if (ItemDescriptionText)
	{
		ItemDescriptionText->SetText(Description);
	}
	
	if (ItemIconImage)
	{
		ItemIconImage->SetBrushFromTexture(Icon);
	}
}
