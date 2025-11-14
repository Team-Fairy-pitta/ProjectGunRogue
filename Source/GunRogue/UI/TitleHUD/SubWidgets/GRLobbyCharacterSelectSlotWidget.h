// GRLobbyCharacterSelectSlotWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRLobbyCharacterSelectSlotWidget.generated.h"

class UButton;
class UTextBlock;
class UBorder;
class USizeBox;
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnCharacterSelectClicked, int32, CharacterIndex);

UCLASS()
class GUNROGUE_API UGRLobbyCharacterSelectSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UBorder* GetBorder() const { return CharSelectButtonBorder; }

	USizeBox* GetSizeBox() const { return CharSelectSizeBox; }

protected:
	UPROPERTY(meta = (BindWidget))
	UButton* CharSelectButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* CharNameText;

	UPROPERTY(meta = (BindWidget))
	UBorder* CharSelectButtonBorder;

	UPROPERTY(meta = (BindWidget))
	USizeBox* CharSelectSizeBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Text")
	FText DefaultText;
	
	virtual void NativePreConstruct() override;
	
	virtual void NativeConstruct() override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Button Event")
	bool bIsClicked = false;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int32 CharacterIndex;

	UPROPERTY(BlueprintAssignable, Category = Event)
	FOnCharacterSelectClicked OnCharacterSelectClicked;

private:
	UFUNCTION()
	void OnCharSelectClicked();
};
