#pragma once

#include "AdvancedFriendsGameInstance.h"
#include "Character/GRCharacter.h"
#include "GRGameInstance.generated.h"

class APlayerController;
class AGRCharacter;

UCLASS()
class GUNROGUE_API UGRGameInstance : public UAdvancedFriendsGameInstance
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintNativeEvent)
	void CreateSession();

#pragma region CharacterSelection
	void RegistPlayerIndex(APlayerController* Player);
	void UnregistPlayerIndex(APlayerController* Player);
	int32 GetPlayerIndex(APlayerController* Player) const;

	void SetSelectedCharacterClass(APlayerController* Player, TSubclassOf<AGRCharacter> PawnClass);
	TSubclassOf<AGRCharacter> GetSelectedCharacterClass(APlayerController* Player) const;

protected:
	UPROPERTY()
	TArray<FString> PlayerIndexArray;

	UPROPERTY()
	TMap<FString, TSubclassOf<AGRCharacter>> SelectedCharacterMap;

private:
	bool GetPlayerUniqueID(APlayerController* Player, FString& OUT PlayerID) const;
#pragma endregion
};
