#pragma once

#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "NativeGameplayTags.h"
#include "GRInputHandleComponent.generated.h"

class AGRCharacter;
struct FInputActionValue;

namespace GunRogue::InputTag
{
	GUNROGUE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
	GUNROGUE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Mouse);
	GUNROGUE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Crouch);

	GUNROGUE_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_InputBlocked);
}

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUNROGUE_API UGRInputHandleComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGRInputHandleComponent();
	virtual void BeginPlay() override;

	void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent);

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);
	void Input_Move(const FInputActionValue& InputActionValue);
	void Input_LookMouse(const FInputActionValue& InputActionValue);
	void Input_Crouch(const FInputActionValue& InputActionValue);


protected:
	AGRCharacter* GetOwningCharacter();
};
