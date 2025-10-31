#include "Input/GRInputConfig.h"

UGRInputConfig::UGRInputConfig(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UInputAction* UGRInputConfig::FindNativeInputActionByTag(const FGameplayTag& InputTag) const
{
	for (const FInputActionWithTag& Element : NativeInputActions)
	{
		if (Element.InputAction && Element.InputTag.MatchesTagExact(InputTag))
		{
			return Element.InputAction;
		}
	}
	return nullptr;
}

const UInputAction* UGRInputConfig::FindAbilityInputActionByTag(const FGameplayTag& InputTag) const
{
	for (const FInputActionWithTag& Element : AbilityInputActions)
	{
		if (Element.InputAction && Element.InputTag.MatchesTagExact(InputTag))
		{
			return Element.InputAction;
		}
	}
	return nullptr;
}
