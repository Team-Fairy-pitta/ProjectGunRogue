#pragma once

#include "AbilitySystem/GRGameplayAbility.h"
#include "GRGameplayAbility_ToggleInventory.generated.h"

UCLASS()
class GUNROGUE_API UGRGameplayAbility_ToggleInventory : public UGRGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGRGameplayAbility_ToggleInventory();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* OwnerInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
};
