// GRAugmentSubsystem.h

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "GRAugmentSubsystem.generated.h"

class UGRAugmentDefinition;
class AGRPlayerState;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRAugmentSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UPROPERTY()
	FSoftObjectPath AugmentDataTablePath;
	
	UGRAugmentDefinition* GetAugment(FName AugmentID) const;

	TArray<UGRAugmentDefinition*> GetRandomAugments(FName CharacterType, int32 Count, AGRPlayerState* PS) const;

private:
	UPROPERTY()
	TMap<FName, TObjectPtr<UGRAugmentDefinition>> AugmentMap;
};
