// GRLobbyBaseButtonWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRLobbyBaseButtonWidget.generated.h"

class UButton;
class UTextBlock;
class UBorder;
class UImage;
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLobbyButtonClicked);

UCLASS()
class GUNROGUE_API UGRLobbyBaseButtonWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UButton* BaseButton;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* BaseButtonText;

	UPROPERTY(meta = (BindWidget))
	UImage* BottomBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BaseButton")
	FText DefaultText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="BaseButton")
	FLinearColor DefaultColor;
	
	virtual void NativePreConstruct() override;
	
	virtual void NativeConstruct() override;

public:
	UPROPERTY(BlueprintAssignable)
	FOnLobbyButtonClicked OnLobbyButtonClicked;
	
private:
	UFUNCTION()
	void OnBaseClicked();
};
