#pragma once

#include "GameFramework/PlayerController.h"
#include "GRPlayerController.generated.h"

class UInputMappingContext;
class UGameplayEffect;

UCLASS()
class GUNROGUE_API AGRPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;

	// Key Setting에 사용할 IMC 목록
	UPROPERTY(EditDefaultsOnly)
	TArray<TObjectPtr<UInputMappingContext>> MappableMappingContexts;

	// Duration이 있는 (무제한 포함) Effect가 추가 되었을 때 호출됨 (Instance는 호출되지 않음)
	UFUNCTION(Client, Reliable)
	void ClientRPC_OnActiveGameplayEffectAdded(TSubclassOf<UGameplayEffect> EffectClass);

	// Duration이 있는 (무제한 포함) Effect가 제거 되었을 때 호출됨 (Instance는 호출되지 않음)
	UFUNCTION(Client, Reliable)
	void ClientRPC_OnActiveGameplayEffectRemoved(TSubclassOf<UGameplayEffect> EffectClass);
};
