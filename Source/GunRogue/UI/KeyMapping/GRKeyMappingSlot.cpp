#include "UI/KeyMapping/GRKeyMappingSlot.h"
#include "UI/KeyMapping/GRKeyMappingWidget.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"

UGRKeyMappingSlot::UGRKeyMappingSlot(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UGRKeyMappingSlot::Init(const FName& InActionName, const FName& InKeyName, UGRKeyMappingWidget* Parent)
{
	ParentWidget = Parent;
	SlotActionName = InActionName;

	if (ActionNameText)
	{
		ActionNameText->SetText(FText::FromName(InActionName));
	}
	if (KeyNameText)
	{
		KeyNameText->SetText(FText::FromName(InKeyName));
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
		const FKey& NewKey = InKeyEvent.GetKey();
		ParentWidget->ChangeKeyMapping(SlotActionName, NewKey);
		ParentWidget->EndChange();
		UnfocusChangeButton();

		if (KeyNameText)
		{
			KeyNameText->SetText(FText::FromName(NewKey.GetFName()));
		}

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
