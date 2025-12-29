#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "GRPawnData.generated.h"

class UGRInputConfig;
class UGRAbilitySet;
class UTexture2D;


USTRUCT(BlueprintType)
struct FGRCharacterSkillInfo
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UTexture2D> SkillIcon;

	// 스킬의 쿨다운 GameplayEffect에 사용된 Tag를 지정해야 합니다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag SkillCooldownTag;
};

// 캐릭터의 입력, 어빌리티, 외형 등을 정의하는 주요 DataAsset 클래스
UCLASS(BlueprintType, Const)
class GUNROGUE_API UGRPawnData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	// 캐릭터를 구분하는 ID
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|ID")
	FName CharacterID;

	// Character의 입력 설정
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Inputs")
	TObjectPtr<UGRInputConfig> InputConfig;

	// Character에 부여할(grand) Ability 정보
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay Ability System")
	TArray<TObjectPtr<UGRAbilitySet>> AbilitySets;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Description")
	FText CharacterName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Description")
	TObjectPtr<UTexture2D> CharacterThumbnail;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|SpawnInLobby")
	TSubclassOf<AActor> CharacterActorInLobby;

	// 스킬 정보 (HUD 업데이트 전용)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Skill")
	FGRCharacterSkillInfo SkillInfo_Q;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character|Skill")
	FGRCharacterSkillInfo SkillInfo_E;
};
