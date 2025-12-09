#include "Player/GRPlayerController.h"
#include "Player/GRPlayerState.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/GRGameplayEffect.h"

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
