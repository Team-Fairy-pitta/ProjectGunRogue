#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GRGameplayAbility.h"
#include "GRGameplayAbility_Attack.generated.h"

class UGameplayEffect;

UCLASS()
class GUNROGUE_API UGRGameplayAbility_Attack : public UGRGameplayAbility
{
	GENERATED_BODY()

	UGRGameplayAbility_Attack();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

protected:
	// 적용할 피해 Effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire")
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire")
	float FireRange = 3000.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire")
	float Damage = 15.0f;

	// 디버그 라인 표시 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire|Debug")
	float DebugLineDuration = 2.0f;

	void FireLineTrace();
};
