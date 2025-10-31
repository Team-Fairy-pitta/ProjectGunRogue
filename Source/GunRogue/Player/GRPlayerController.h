#pragma once

#include "GameFramework/PlayerController.h"
#include "GRPlayerController.generated.h"

UCLASS()
class GUNROGUE_API AGRPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:

	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
};
