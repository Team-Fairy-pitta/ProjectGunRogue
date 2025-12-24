#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GRGameplayAbility.h"
#include "GameplayTagContainer.h"
#include "GRGameplayAbility_BladeWaveMode.generated.h"


UCLASS()
class GUNROGUE_API UGRGameplayAbility_BladeWaveMode : public UGRGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGRGameplayAbility_BladeWaveMode();

protected:
	UPROPERTY(EditDefaultsOnly, Category = "BladeWave|Mode")
	float ModeDuration = 6.0f;

	UPROPERTY(EditDefaultsOnly, Category = "BladeWave|Mode")
	FGameplayTag Tag_BladeWaveMode;

	UPROPERTY(EditDefaultsOnly, Category = "BladeWave|Animation")
	UAnimMontage* AvtiavteMontage = nullptr;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

private:
	UFUNCTION()
	void OnModeEnded();
};
