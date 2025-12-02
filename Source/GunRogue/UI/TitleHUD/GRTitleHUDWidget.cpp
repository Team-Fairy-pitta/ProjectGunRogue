#include "UI/TitleHUD/GRTitleHUDWidget.h"
#include "SubWidgets/GRTitleMenuButtonWidget.h"
#include "Player/GameStart/GRGameStart_PlayerController.h"

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

void UGRTitleHUDWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (StartGameButton)
	{
		StartGameButton->OnTitleButtonClicked.RemoveDynamic(this, &UGRTitleHUDWidget::OnStartGameClicked);
	}
	if (CheckInvitationButton)
	{
		CheckInvitationButton->OnTitleButtonClicked.RemoveDynamic(this, &UGRTitleHUDWidget::OnCheckInvitationClicked);
	}
	if (SetGameButton)
	{
		SetGameButton->OnTitleButtonClicked.RemoveDynamic(this, &UGRTitleHUDWidget::OnSetGameClicked);
	}
	if (ExitGameButton)
	{
		ExitGameButton->OnTitleButtonClicked.RemoveDynamic(this, &UGRTitleHUDWidget::OnExitGameClicked);
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
	AGRGameStart_PlayerController* Controller = GetOwningPlayer<AGRGameStart_PlayerController>();
	if (!IsValid(Controller))
	{
		UE_LOG(LogTemp, Error, TEXT("Controller is NOT GameStart PC"));
		return;
	}

	Controller->ShowSettingWidget();
}

void UGRTitleHUDWidget::OnExitGameClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Exit Game Clicked"));
}
