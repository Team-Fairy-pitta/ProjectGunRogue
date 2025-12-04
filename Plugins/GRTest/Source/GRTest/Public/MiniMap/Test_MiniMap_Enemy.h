#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Test_MiniMap_Enemy.generated.h"

class UAbilitySystemComponent;

UCLASS()
class GRTEST_API ATest_MiniMap_Enemy : public AActor
{
	GENERATED_BODY()
	
public:	
	ATest_MiniMap_Enemy();

protected:
	virtual void BeginPlay() override;

public:	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAbilitySystemComponent* AbilitySystem;

};
