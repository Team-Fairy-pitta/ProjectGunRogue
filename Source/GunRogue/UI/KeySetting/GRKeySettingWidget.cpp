#include "UI/KeySetting/GRKeySettingWidget.h"
#include "EnhancedInputSubsystems.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "GameFramework/PlayerController.h"

void UGRKeySettingWidget::InitKeyMappings()
{
	SetupUserSetting();
}

void UGRKeySettingWidget::SetupUserSetting()
{
	APlayerController* PlayerController = GetOwningPlayer();
	if (!IsValid(PlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("Widget Owner(Player Controller) is INVALID."));
		return;
	}

	ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer();
	if (!LocalPlayer)
	{
		UE_LOG(LogTemp, Error, TEXT("LocalPlayer (PlayerController->GetLocalPlayer()) is INVALID."));
		return;
	}

	CachedUserSetting = UEnhancedInputUserSettings::LoadOrCreateSettings(LocalPlayer);
	if (!CachedUserSetting)
	{
		UE_LOG(LogTemp, Error, TEXT("CachedUserSetting (UEnhancedInputUserSettings) is INVALID."));
		return;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!Subsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("Subsystem (UEnhancedInputLocalPlayerSubsystem) is INVALID."));
		return;
	}

	//CachedUserSetting->RegisterInputMappingContext();
}
