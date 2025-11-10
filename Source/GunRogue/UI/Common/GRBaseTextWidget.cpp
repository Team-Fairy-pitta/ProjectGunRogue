// GRBaseTextWidget.cpp


#include "GRBaseTextWidget.h"
#include "Components/TextBlock.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"

void UGRBaseTextWidget::NativePreConstruct()
{
	Super::NativePreConstruct();

	if (Text.IsEmpty())
	{
		Text = FText::FromString(TEXT(""));
	}

	if (!FontInfo.FontObject)
	{
		FontInfo.FontObject = GEngine->GetSmallFont();
		FontInfo.TypefaceFontName = FName("Bold");
		FontInfo.Size = 32;	
	}

	if (!TextColor.IsColorSpecified())
	{
		TextColor = FSlateColor(FLinearColor::White);
	}

	SyncTextAndFont();
}

void UGRBaseTextWidget::SetText(const FText& InText)
{
	Text = InText;
	SyncTextAndFont();
}

void UGRBaseTextWidget::SetColor(const FSlateColor& InTextColor)
{
	TextColor = InTextColor;
	SyncTextAndFont();
}

void UGRBaseTextWidget::SyncTextAndFont()
{
	if (TextBox)
	{
		TextBox->SetText(Text);
		TextBox->SetFont(FontInfo);
		TextBox->SetColorAndOpacity(TextColor);
	}
}
