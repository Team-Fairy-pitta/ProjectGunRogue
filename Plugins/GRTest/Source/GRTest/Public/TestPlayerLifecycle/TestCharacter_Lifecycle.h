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

	

private:
	UFUNCTION(Server, Reliable)
	void ServerDie();

	void Die_Internal();


};
