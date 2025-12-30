#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/GRGameplayAbility.h"
#include "GameplayTagContainer.h"
#include "Character/Attachment/GRCharacterAttachment.h"
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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "BladeWave|Mesh")
	FGRCharacterAttachment SwordAttachment;

	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

private:
	UFUNCTION()
	void OnModeEnded();

	bool CanActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayTagContainer* SourceTags,
		const FGameplayTagContainer* TargetTags,
		FGameplayTagContainer* OptionalRelevantTags) const;

	void AttachSwordToCharacter();
	void RollbackAttachment();

	UPROPERTY(Transient)
	FGRCharacterAttachmentHandle SwordAttachmentHandle;
};
