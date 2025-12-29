#pragma once

#include "AbilitySystem/GRGameplayAbility.h"
#include "GRGameplayAbility_InGameMenu.generated.h"

UCLASS()
class GUNROGUE_API UGRGameplayAbility_InGameMenu : public UGRGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGRGameplayAbility_InGameMenu();

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* OwnerInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;
};
