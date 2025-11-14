#pragma once

#include "GameFramework/PlayerController.h"
#include "GRPlayerController.generated.h"

class UInputMappingContext;

UCLASS()
class GUNROGUE_API AGRPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

	// Key Setting에 사용할 IMC 목록
	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UInputMappingContext>> MappableMappingContexts;
};
