#pragma once

#include "AbilitySystem/GRGameplayAbility.h"
#include "GRGameplayAbility_InGameMenu.generated.h"

UCLASS()
class GUNROGUE_API UGRGameplayAbility_InGameMenu : public UGRGameplayAbility
{
	GENERATED_BODY()
	
public:

	virtual bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags = nullptr,
		const FGameplayTagContainer* TargetTags = nullptr,
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr
	) const override;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* OwnerInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
};
