// GRTitleHUDWidget.cpp


#include "UI/TitleHUD/GRTitleHUDWidget.h"

#include "SubWidgets/GRTitleMenuButtonWidget.h"

void UGRTitleHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (StartGameButton)
	{
		StartGameButton->OnTitleButtonClicked.AddDynamic(this, &UGRTitleHUDWidget::OnStartGameClicked);
	}
	if (CheckInvitationButton)
	{
		CheckInvitationButton->OnTitleButtonClicked.AddDynamic(this, &UGRTitleHUDWidget::OnCheckInvitationClicked);
	}
	if (SetGameButton)
	{
		SetGameButton->OnTitleButtonClicked.AddDynamic(this, &UGRTitleHUDWidget::OnSetGameClicked);
	}
	if (ExitGameButton)
	{
		ExitGameButton->OnTitleButtonClicked.AddDynamic(this, &UGRTitleHUDWidget::OnExitGameClicked);
	}
}

void UGRTitleHUDWidget::OnStartGameClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Start Game Clicked"));
}

void UGRTitleHUDWidget::OnCheckInvitationClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Check Invitation Clicked"));
}

void UGRTitleHUDWidget::OnSetGameClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Set Game Clicked"));
}

void UGRTitleHUDWidget::OnExitGameClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Exit Game Clicked"));
}
