#include "UI/InGame/GRInGameHUDWidget.h"

#include "Components/Button.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Player/Battle/GRBattlePlayerController.h"


void UGRInGameHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	SetWidgetFocusable();

	if (GameOverButton)
	{
		GameOverButton->OnClicked.AddUniqueDynamic(this, &UGRInGameHUDWidget::OnGameOver);
	}

	if (StartLobbyButton)
	{
		StartLobbyButton->OnClicked.AddUniqueDynamic(this, &UGRInGameHUDWidget::OnStartLobby);
	}

	if (BackToMenuButton)
	{
		BackToMenuButton->OnClicked.AddUniqueDynamic(this, &UGRInGameHUDWidget::OnBackMenu);
	}
}

void UGRInGameHUDWidget::NativeDestruct()
{
	if (GameOverButton)
	{
		GameOverButton->OnClicked.RemoveDynamic(this, &UGRInGameHUDWidget::OnGameOver);
	}

	if (StartLobbyButton)
	{
		StartLobbyButton->OnClicked.RemoveDynamic(this, &UGRInGameHUDWidget::OnStartLobby);
	}

	if (BackToMenuButton)
	{
		BackToMenuButton->OnClicked.RemoveDynamic(this, &UGRInGameHUDWidget::OnBackMenu);
	}

	
	Super::NativeDestruct();
}

FReply UGRInGameHUDWidget::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		if (APlayerController* PC = GetOwningPlayer())
		{
			if (AGRBattlePlayerController* BattlePC = Cast<AGRBattlePlayerController>(PC))
			{
				BattlePC->HideInGameMenuWidget();
				return FReply::Handled();
			}
		}
	}

	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
}

void UGRInGameHUDWidget::OnGameOver()
{
	UKismetSystemLibrary::QuitGame(GetWorld(), GetOwningPlayer(), EQuitPreference::Quit, true);
}

void UGRInGameHUDWidget::OnStartLobby()
{
	UGameplayStatics::OpenLevel(this, FName("/Game/GunRogue/Map/GameStart/GameStartMap"));
}

void UGRInGameHUDWidget::OnBackMenu()
{
	AGRBattlePlayerController* BattlePC = GetOwningPlayer<AGRBattlePlayerController>();
	if (!IsValid(BattlePC))
	{
		return;
	}

	BattlePC->HideInGameMenuWidget();
}

void UGRInGameHUDWidget::SetWidgetFocusable()
{
	SetIsFocusable(true);
}
