#pragma once

#include "CoreMinimal.h"
#include "Character/GRCharacter.h"
#include "TestCharacter_Lifecycle.generated.h"

UCLASS()
class GRTEST_API ATestCharacter_Lifecycle : public AGRCharacter
{
	GENERATED_BODY()

public:
	ATestCharacter_Lifecycle();

	UFUNCTION(BlueprintCallable)
	void Die();

	bool IsDead() const { return bIsDead; }

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

private:
	UPROPERTY(Replicated)
	bool bIsDead = false;
};
