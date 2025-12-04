#include "UI/TitleHUD/GRTitleHUDWidget.h"
#include "SubWidgets/GRTitleMenuButtonWidget.h"
#include "Player/GameStart/GRGameStart_PlayerController.h"
#include "System/GRGameInstance.h"
#include "Kismet/KismetSystemLibrary.h"

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
	UGRGameInstance* GRGameInstance = GetGameInstance<UGRGameInstance>();
	if (!GRGameInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("GRGameInstance is INVALID"));
		return;
	}

	GRGameInstance->CreateSession();
}

void UGRTitleHUDWidget::OnCheckInvitationClicked()
{
	// [NOTE] 초대 확인 비활성화
	// Steam overlay로 충분히 스팀 초대를 받을 수 있기 때문에, 구현 우선 순위를 최하위로 낮추겠습니다.
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
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, true);
}
