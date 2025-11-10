// GRTitleNavigationWidget.cpp


#include "UI/TitleHUD/SubWidgets/GRTitleNavigationWidget.h"
#include "Components/Button.h"

void UGRTitleNavigationWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (StartGameButton)
	{
		StartGameButton->OnClicked.AddDynamic(this, &UGRTitleNavigationWidget::OnStartGameClicked);
	}
	if (CheckInvitationButton)
	{
		CheckInvitationButton->OnClicked.AddDynamic(this, &UGRTitleNavigationWidget::OnCheckInvitationClicked);
	}
	if (SetGameButton)
	{
		SetGameButton->OnClicked.AddDynamic(this, &UGRTitleNavigationWidget::OnSetGameClicked);
	}
	if (ExitGameButton)
	{
		ExitGameButton->OnClicked.AddDynamic(this, &UGRTitleNavigationWidget::OnExitGameClicked);
	}
	
}

void UGRTitleNavigationWidget::OnStartGameClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Start Game Button Clicked!"));
}

void UGRTitleNavigationWidget::OnCheckInvitationClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Click Invitaion Button Clicked!"));
}

void UGRTitleNavigationWidget::OnSetGameClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Set Game Button Clicked!"));
}

void UGRTitleNavigationWidget::OnExitGameClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("Exit Game Button Clicked!"));
}
