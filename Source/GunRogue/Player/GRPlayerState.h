#pragma once

#include "GameFramework/PlayerState.h"
#include "AbilitySystemInterface.h"
#include "GRPlayerState.generated.h"

class AGRPlayerController;
class AGRCharacter;
class UGRAbilitySystemComponent;

UCLASS()
class GUNROGUE_API AGRPlayerState : public APlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AGRPlayerState();
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "ITPlayerState")
	AGRPlayerController* GetGRPlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "ITPlayerState")
	AGRCharacter* GetGRCharacter() const;

	UFUNCTION(BlueprintCallable, Category = "ITPlayerState")
	UGRAbilitySystemComponent* GetGRAbilitySystemComponent();

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	UPROPERTY(VisibleAnywhere, Category = "ITPlayerState|AbilitySystemComponent")
	TObjectPtr<UGRAbilitySystemComponent> AbilitySystemComponent;
};
