#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "GRRadarTagComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GUNROGUE_API UGRRadarTagComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UGRRadarTagComponent();

protected:
	virtual void BeginPlay() override;

public:	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Minimap", Meta = (Categories = "MiniMap"))
	FGameplayTag RadarTag;
};
