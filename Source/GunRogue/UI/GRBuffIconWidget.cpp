// GRBuffIconWidget.cpp


#include "UI/GRBuffIconWidget.h"
#include "Components/Image.h"


void UGRBuffIconWidget::SetBuffIcon(UTexture2D* NewIcon)
{
	if (!BuffIconImage || !NewIcon) return;

	BuffIconImage->SetBrushFromTexture(NewIcon);
}
