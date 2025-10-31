#pragma once

#include "EnhancedInputComponent.h"
#include "Input/GRInputConfig.h"
#include "GRInputComponent.generated.h"

UCLASS()
class GUNROGUE_API UGRInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()
	
public:
	template<class UserClass, typename FuncType>
	void BindNativeAction(
		const UGRInputConfig* InputConfig, 
		const FGameplayTag& InputTag, 
		ETriggerEvent TriggerEvent, 
		UserClass* Object, 
		FuncType Func);

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(
		const UGRInputConfig* InputConfig, 
		UserClass* Object, 
		PressedFuncType PressedFunc, 
		ReleasedFuncType ReleasedFunc, 
		TArray<uint32>& BindHandles);

};

template<class UserClass, typename FuncType>
inline void UGRInputComponent::BindNativeAction(
	const UGRInputConfig* InputConfig, 
	const FGameplayTag& InputTag, 
	ETriggerEvent TriggerEvent, 
	UserClass* Object, 
	FuncType Func)
{
	check(InputConfig);

	if (const UInputAction* InputAction = InputConfig->FindNativeInputActionByTag(InputTag))
	{
		BindAction(InputAction, TriggerEvent, Object, Func);
	}
}

template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
inline void UGRInputComponent::BindAbilityActions(
	const UGRInputConfig* InputConfig, 
	UserClass* Object, 
	PressedFuncType PressedFunc, 
	ReleasedFuncType ReleasedFunc, 
	TArray<uint32>& BindHandles)
{
	check(InputConfig);

	for (const FInputActionWithTag& AbilityInputAction : InputConfig->AbilityInputActions)
	{
		const UInputAction* InputAction = AbilityInputAction.InputAction;
		const FGameplayTag& InputTag = AbilityInputAction.InputTag;
		if (InputAction && InputTag.IsValid())
		{
			if (PressedFunc)
			{
				uint32 PressedBindHandle = BindAction(InputAction, ETriggerEvent::Triggered, Object, PressedFunc, InputTag).GetHandle();
				BindHandles.Add(PressedBindHandle);
			}

			if (ReleasedFunc)
			{
				uint32 ReleasedBindHandle = BindAction(InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, InputTag).GetHandle();
				BindHandles.Add(ReleasedBindHandle);
			}
		}
	}
}
