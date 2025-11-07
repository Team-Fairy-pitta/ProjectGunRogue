// GRBaseTextWidget.cpp


#include "UI/GRBaseTextWidget.h"
#include "Components/TextBlock.h"
#include "Engine/Engine.h"
#include "Engine/Font.h"

void UGRBaseTextWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (Text.IsEmpty())
	{
		Text = FText::FromString(TEXT("TEXT"));
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

void UGRBaseTextWidget::SetTextAndColor(const FText& InText, const FSlateColor& InTextColor)
{
	Text = InText;
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
