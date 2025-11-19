#include "TestAnimation/BP/TestGRCharacter.h"

#include "AbilitySystemComponent.h"
#include "TestAnimation/GA/TestGAEquipMontage.h"

ATestGRCharacter::ATestGRCharacter()
{
	AbilitySystem = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystem"));
}

UAbilitySystemComponent* ATestGRCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystem;
}

void ATestGRCharacter::BeginPlay()
{
	Super::BeginPlay();

	AbilitySystem->InitAbilityActorInfo(this, this);

	FGameplayAbilitySpec Spec(UTestGAEquipMontage::StaticClass(), 1, 0);
	AbilitySystem->GiveAbility(Spec);
}
