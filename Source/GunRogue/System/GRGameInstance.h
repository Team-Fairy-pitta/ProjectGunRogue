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
	void SetSelectedCharacterClass(APlayerController* Player, TSubclassOf<AGRCharacter> PawnClass);
	TSubclassOf<AGRCharacter> GetSelectedCharacterClass(APlayerController* Player) const;

protected:
	TMap<FUniqueNetIdRepl, TSubclassOf<AGRCharacter>> SelectedCharacterMap;
#pragma endregion
};
