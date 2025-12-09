#include "UI/TitleHUD/GRSettingWidget.h"
#include "UI/KeySetting/GRKeySettingWidget.h"
#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Player/GameStart/GRGameStart_PlayerController.h"

void UGRSettingWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (TabButton_Game)
	{
		if (!TabButton_Game->OnClicked.IsAlreadyBound(this, &ThisClass::OnClicked_TabButton_Game))
		{
			TabButton_Game->OnClicked.AddDynamic(this, &ThisClass::OnClicked_TabButton_Game);
		}
	}
	if (TabButton_Key)
	{
		if (!TabButton_Key->OnClicked.IsAlreadyBound(this, &ThisClass::OnClicked_TabButton_Key))
		{
			TabButton_Key->OnClicked.AddDynamic(this, &ThisClass::OnClicked_TabButton_Key);
		}
	}
	if (TabButton_Other)
	{
		if (!TabButton_Other->OnClicked.IsAlreadyBound(this, &ThisClass::OnClicked_TabButton_Other))
		{
			TabButton_Other->OnClicked.AddDynamic(this, &ThisClass::OnClicked_TabButton_Other);
		}
	}
	if (BackToMenuButton)
	{
		if (!BackToMenuButton->OnClicked.IsAlreadyBound(this, &ThisClass::OnClicked_BackToMenuButton))
		{
			BackToMenuButton->OnClicked.AddDynamic(this, &ThisClass::OnClicked_BackToMenuButton);
		}
	}

	if (KeySettingWidget)
	{
		KeySettingWidget->InitKeyMappings();
	}

	OnClicked_TabButton_Game();
}

void UGRSettingWidget::NativeDestruct()
{
	Super::NativeDestruct();

	if (TabButton_Game)
	{
		if (TabButton_Game->OnClicked.IsAlreadyBound(this, &ThisClass::OnClicked_TabButton_Game))
		{
			TabButton_Game->OnClicked.RemoveDynamic(this, &ThisClass::OnClicked_TabButton_Game);
		}
	}
	if (TabButton_Key)
	{
		if (TabButton_Key->OnClicked.IsAlreadyBound(this, &ThisClass::OnClicked_TabButton_Key))
		{
			TabButton_Key->OnClicked.RemoveDynamic(this, &ThisClass::OnClicked_TabButton_Key);
		}
	}
	if (TabButton_Other)
	{
		if (TabButton_Other->OnClicked.IsAlreadyBound(this, &ThisClass::OnClicked_TabButton_Other))
		{
			TabButton_Other->OnClicked.RemoveDynamic(this, &ThisClass::OnClicked_TabButton_Other);
		}
	}
	if (BackToMenuButton)
	{
		if (BackToMenuButton->OnClicked.IsAlreadyBound(this, &ThisClass::OnClicked_BackToMenuButton))
		{
			BackToMenuButton->OnClicked.RemoveDynamic(this, &ThisClass::OnClicked_BackToMenuButton);
		}
	}
}

void UGRSettingWidget::SetTabButtonColor(int32 Index)
{
	if (TabButton_Game)
	{
		FLinearColor& TargetColor = Index == 0 ? TabActiveBackgroundColor : TabInactiveBackgroundColor;
		TabButton_Game->SetBackgroundColor(TargetColor);
	}
	if (TabButton_Key)
	{
		FLinearColor& TargetColor = Index == 1 ? TabActiveBackgroundColor : TabInactiveBackgroundColor;
		TabButton_Key->SetBackgroundColor(TargetColor);
	}
	if (TabButton_Other)
	{
		FLinearColor& TargetColor = Index == 2 ? TabActiveBackgroundColor : TabInactiveBackgroundColor;
		TabButton_Other->SetBackgroundColor(TargetColor);
	}
}

void UGRSettingWidget::OnClicked_BackToMenuButton()
{
	AGRGameStart_PlayerController* Controller = GetOwningPlayer<AGRGameStart_PlayerController>();
	if (!IsValid(Controller))
	{
		UE_LOG(LogTemp, Error, TEXT("Controller is NOT GameStart PC"));
		return;
	}

	Controller->HideSettingWidget();
}

void UGRSettingWidget::OnClicked_TabButton_Game()
{
	if (!SettingTab)
	{
		return;
	}

	SettingTab->SetActiveWidgetIndex(0);
	SetTabButtonColor(0);
}

void UGRSettingWidget::OnClicked_TabButton_Key()
{
	if (!SettingTab)
	{
		return;
	}

	SettingTab->SetActiveWidgetIndex(1);
	SetTabButtonColor(1);
}

void UGRSettingWidget::OnClicked_TabButton_Other()
{
	if (!SettingTab)
	{
		return;
	}

	SettingTab->SetActiveWidgetIndex(2);
	SetTabButtonColor(2);
}
