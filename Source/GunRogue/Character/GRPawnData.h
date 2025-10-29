#pragma once

#include "Engine/DataAsset.h"
#include "GRPawnData.generated.h"

class UGRInputConfig;
class UGRAbilitySet;

// 캐릭터의 입력, 어빌리티, 외형 등을 정의하는 주요 DataAsset 클래스
UCLASS(BlueprintType, Const)
class GUNROGUE_API UGRPawnData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// Character의 입력 설정
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs")
	TObjectPtr<UGRInputConfig> InputConfig;

	// Character에 부여할(grand) Ability 정보
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Ability System")
	TArray<TObjectPtr<UGRAbilitySet>> AbilitySets;
};
