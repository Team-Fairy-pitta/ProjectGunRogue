#include "Player/GRPlayerController.h"
#include "Player/GRPlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/GRGameplayEffect.h"
#include "Framework/Application/SlateApplication.h"
#include "Framework/Application/NavigationConfig.h"

void AGRPlayerController::BeginPlay()
{
	TSharedRef<FNavigationConfig> NavigationConfig = FSlateApplication::Get().GetNavigationConfig();
	NavigationConfig->bTabNavigation = false;
	NavigationConfig->bKeyNavigation = false;
	NavigationConfig->bAnalogNavigation = false;
}

void AGRPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	AGRPlayerState* GRPlayerState = GetPlayerState<AGRPlayerState>();
	if (!IsValid(GRPlayerState))
	{
		return;
	}

	UGRAbilitySystemComponent* ASC = GRPlayerState->GetGRAbilitySystemComponent();
	if (!ASC)
	{
		return;
	}
	ASC->ProcessAbilityInput(DeltaTime, bGamePaused);
}
