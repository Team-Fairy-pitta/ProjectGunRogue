#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GRGameplayAbility.h"
#include "GRGameplayAbility_HitscanAttack.generated.h"

class UGameplayEffect;

UCLASS()
class GUNROGUE_API UGRGameplayAbility_HitscanAttack : public UGRGameplayAbility
{
	GENERATED_BODY()

	UGRGameplayAbility_HitscanAttack();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

	virtual void InputReleased(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;

protected:
	// 적용할 피해 Effect
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire")
	TSubclassOf<UGameplayEffect> DamageEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire")
	float FireRange = 20000.0f; // 매직 넘버. 실내 전투라 사거리 중요하지 않음.

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire")
	float FallbackDamage = 15.0f; // CombatAttribute 없을 때만 사용

	// 디버그 라인 표시 시간
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Fire|Debug")
	float DebugLineDuration = 2.0f;

	void FireLineTrace();

	// 연사 시작/중지
	void StartContinuousFire();
	void StopContinuousFire();

	// 연사 타이머
	FTimerHandle FireTimerHandle;

	void ApplyRecoil(APlayerController* PC, float RecoilAmount);
};
