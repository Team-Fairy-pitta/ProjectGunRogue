#pragma once

#include "Engine/DataAsset.h"
#include "TestLocationDataAsset.generated.h"

USTRUCT(BlueprintType)
struct FNamedLocation
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName LocationName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FVector Location;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FRotator Rotation;
};


UCLASS()
class GRTEST_API UTestLocationDataAsset : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FNamedLocation> Locations;
};
