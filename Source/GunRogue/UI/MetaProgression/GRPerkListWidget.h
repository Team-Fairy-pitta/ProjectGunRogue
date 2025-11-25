// GRPerkListWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRPerkListWidget.generated.h"

class UHorizontalBox;
class UGRPerkSlotWidget;
class UGRPerkSubsystem;
class UTextBlock;
class UGRPerkHUDWidget;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRPerkListWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	UPROPERTY(meta = (BindWidget))
	UHorizontalBox* PerkSlotContainer;

	UPROPERTY(meta = (BindWidget))
	UTextBlock* PerkCategoryText;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perk")
	TSubclassOf<UGRPerkSlotWidget> PerkSlotClass;

	UPROPERTY()
	UGRPerkSubsystem* PerkSubsystem;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perk")
	UDataTable* PerkTable;

	UPROPERTY()
	UGRPerkHUDWidget* PerkHUD;
	
public:
	void SetupList(UGRPerkSubsystem* InSubsystem, UGRPerkHUDWidget* InHUD);

	void SetPerkCategoryText(FName InCategory);

	void CreateAllSlot(FName InCategory);
};
