#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayTagContainer.h"
#include "Test_MiniMap_Actor.generated.h"

class UAbilitySystemComponent;

UCLASS()
class GRTEST_API ATest_MiniMap_Actor : public AActor
{
	GENERATED_BODY()
	
public:	
	ATest_MiniMap_Actor();

protected:
	virtual void BeginPlay() override;

public:	

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UAbilitySystemComponent* AbilitySystem;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap")
	TArray<FGameplayTag> RadarTag;

};
