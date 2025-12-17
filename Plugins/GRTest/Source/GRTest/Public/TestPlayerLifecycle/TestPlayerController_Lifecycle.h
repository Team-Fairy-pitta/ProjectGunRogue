#pragma once

#include "CoreMinimal.h"
#include "Player/Battle/GRBattlePlayerController.h"
#include "TestPlayerController_Lifecycle.generated.h"

class ATestCharacter_Lifecycle;

UCLASS()
class GRTEST_API ATestPlayerController_Lifecycle : public AGRBattlePlayerController
{
	GENERATED_BODY()
	
public:
	ATestPlayerController_Lifecycle();


	// 관전 모드 시작
	void Spectating();

	UFUNCTION(Server, Reliable)
	void ServerSpectating();

	UFUNCTION()
	void Spectating_Internal();
	
	// 관전 대상 변경
	UFUNCTION(BlueprintCallable)
	void SpectateNext();

	UFUNCTION(Server, Reliable)
	void ServerSpectateNext();

	UFUNCTION()
	void SpectateNext_Internal();
	
	UFUNCTION(BlueprintCallable)
	void SpectatePrevious();

	UFUNCTION(Server, Reliable)
	void ServerSpectatePrevious();

	UFUNCTION()
	void SpectatePrevious_Internal();

	// 리스폰
	UFUNCTION(BlueprintCallable)
	void Respawn();
	
	UFUNCTION(Server, Reliable)
	void ServerRespawn();

	UFUNCTION()
	void Respawn_Internal();

private:

	UFUNCTION(Server, Reliable)
	void ServerSpectateTarget(AActor* Target);

public:
	virtual void BeginPlay() override;

private:
	int32 CurrentIndex = 0;
};
