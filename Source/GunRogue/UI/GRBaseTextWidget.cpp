// GRBaseTextWidget.cpp


#include "UI/GRBaseTextWidget.h"
#include "Components/TextBlock.h"

UGRBaseTextWidget::UGRBaseTextWidget(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Text = FText::FromString(TEXT("TEXT"));

	FontInfo = FSlateFontInfo();
	FontInfo.FontObject = nullptr;
	FontInfo.TypefaceFontName = FName("Bold");
	FontInfo.Size = 32;
	
	TextColor = FSlateColor(FLinearColor::White);
}

void UGRBaseTextWidget::UpdateTextAndFont(const FText& InText, const FSlateFontInfo& InFontInfo,
                                          const FSlateColor& InTextColor)
{
	Text = InText;
	FontInfo = InFontInfo;
	TextColor = InTextColor;

	SyncTextAndFont();
}

void UGRBaseTextWidget::NativeConstruct()
{
	Super::NativeConstruct();

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
