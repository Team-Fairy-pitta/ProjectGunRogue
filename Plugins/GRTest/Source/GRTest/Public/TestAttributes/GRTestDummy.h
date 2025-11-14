#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/Attributes/GRHealthAttributeSet.h"
#include "AbilitySystemComponent.h"
#include "GRTestDummy.generated.h"

class UAbilitySystemComponent;

UCLASS()
class GRTEST_API AGRTestDummy : public AActor, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AGRTestDummy();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override
	{
		return AbilitySystemComponent;
	}

	virtual void BeginPlay() override;

protected:

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UGRHealthAttributeSet> HealthAttributeSet;

	void PrintHealthChanged(AActor* Instigator, AActor* Causer,
		const FGameplayEffectSpec* Spec, float Magnitude, float OldValue, float NewValue);

	void PrintShieldChanged(AActor* Instigator, AActor* Causer,
		const FGameplayEffectSpec* Spec, float Magnitude, float OldValue, float NewValue);

	void PrintShieldBroken(AActor* Instigator, AActor* Causer,
		const FGameplayEffectSpec* Spec, float Magnitude, float OldValue, float NewValue);

	void OnDummyDeath(AActor* Instigator, AActor* Causer, const FGameplayEffectSpec* Spec,
		float Magnitude, float OldValue, float NewValue);
};
