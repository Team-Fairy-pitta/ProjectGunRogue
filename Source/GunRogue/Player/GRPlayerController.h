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

#pragma region Spectator
public:
	UFUNCTION(Server, Reliable)
	void ServerRPC_StartSpectating();

	UFUNCTION(Server, Reliable)
	void ServerRPC_SpectatePreviousPlayer();

	UFUNCTION(Server, Reliable)
	void ServerRPC_SpectateNextPlayer();

protected:
	AActor* GetPreviousSpectateActor();
	AActor* GetNextSpectateActor();
	
	TArray<AActor*> GetAlivePlayerList();

	int32 CurrentSpectateIndex = INDEX_NONE;

#pragma endregion
};
