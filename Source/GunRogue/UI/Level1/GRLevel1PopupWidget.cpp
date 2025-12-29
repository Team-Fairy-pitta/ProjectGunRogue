#include "UI/Level1/GRLevel1PopupWidget.h"
#include "Components/TextBlock.h"

void UGRLevel1PopupWidget::SetPopupText(int32 Index, const FString& StatusString, const FString& TypeString)
{
	if (IndexText)
	{
		FText InText = FText::FromString(FString::Printf(TEXT("구역 번호: %d"), Index));
		IndexText->SetText(InText);
	}

	if (StatusText)
	{
		FText InText = FText::FromString(FString::Printf(TEXT("상태: %s"), *StatusString));
		StatusText->SetText(InText);
	}

	if (TypeText)
	{
		FText InText = FText::FromString(FString::Printf(TEXT("정보: %s"), *TypeString));
		TypeText->SetText(InText);
	}
}
