#include "UI/KeyMapping/GRKeyMappingSlot.h"
#include "UI/KeyMapping/GRKeyMappingWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

UGRKeyMappingSlot::UGRKeyMappingSlot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UGRKeyMappingSlot::Init(const FName& ActionName, const FName& KeyName, UGRKeyMappingWidget* Parent)
{
	ParentWidget = Parent;

	if (ActionNameText)
	{
		ActionNameText->SetText(FText::FromName(ActionName));
	}
	if (KeyNameText)
	{
		KeyNameText->SetText(FText::FromName(KeyName));
	}
	if (MappingChangeButton)
	{
		MappingChangeButton->OnClicked.AddDynamic(this, &ThisClass::OnChangeButtonClicked);
		UnfocusChangeButton();
	}
}

FReply UGRKeyMappingSlot::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	check(ParentWidget);

	if (ParentWidget->IsChanging())
	{
		ParentWidget->EndChange();
		UnfocusChangeButton();
		return FReply::Handled();
	}
	else
	{
		return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
	}
}

void UGRKeyMappingSlot::OnChangeButtonClicked()
{
	check(ParentWidget);

	if (ParentWidget->IsChanging())
	{
		ParentWidget->EndChange();
		ParentWidget->StartChange(this);
		FocusChangeButton();
	}
	else
	{
		ParentWidget->StartChange(this);
		FocusChangeButton();
	}
}

void UGRKeyMappingSlot::FocusChangeButton()
{
	if (MappingChangeButton)
	{
		MappingChangeButton->SetBackgroundColor(FLinearColor::Blue);
	}
}

void UGRKeyMappingSlot::UnfocusChangeButton()
{
	if (MappingChangeButton)
	{
		MappingChangeButton->SetBackgroundColor(FLinearColor::Gray);
	}
}
