#pragma once

#include "Components/ActorComponent.h"
#include "GameplayTagContainer.h"
#include "GRRadarMapComponent.generated.h"

class UGRRadarMapWidget;

USTRUCT(BlueprintType)
struct FRadarTargetInfo
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	AActor* TargetActor = nullptr;

	UPROPERTY(BlueprintReadOnly)
	FVector2D NormalizedTargetDirection = FVector2D::ZeroVector;

	UPROPERTY(BlueprintReadOnly)
	FGameplayTag RadarTag;
};

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class GUNROGUE_API UGRRadarMapComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UGRRadarMapComponent();

	void InitRadarWidget();

	void ScanRadar();

	FVector2D GetNormalizedTargetDirection(FVector TargetLocation) const;

protected:
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Radar")
	float ScanRadius = 2000.f;

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UGRRadarMapWidget> RadarMapWidgetInstance;

	FTimerHandle ScanTimer;
};
