// GRInvitationSlotWidget.cpp


#include "UI/TitleHUD/SubWidgets/GRInvitationSlotWidget.h"
#include "Components/Button.h"
#include "UI/Common/GRBaseTextWidget.h"

void UGRInvitationSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (InvitationButton)
	{
		InvitationButton->OnClicked.AddDynamic(this, &UGRInvitationSlotWidget::OnInvitationClicked);
	}
}

void UGRInvitationSlotWidget::SetInvitationText(const FText& InText)
{
	if (!InvitationText)
	{
		return;
	}

	InvitationText->SetText(InText);
}

void UGRInvitationSlotWidget::OnInvitationClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Invitation Button Clicked!"));
}
