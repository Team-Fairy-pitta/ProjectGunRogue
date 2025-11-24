// GRPerkHUDWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRPerkHUDWidget.generated.h"

class UGRGoodsTextWidget;
class UVerticalBox;
class UGRPerkListWidget;
class UGRPerkSlotWidget;
class UGRPerkSubsystem;
class UGRPerkTooltipWidget;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRPerkHUDWidget : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	UPROPERTY(meta = (BindWidget))
	UGRGoodsTextWidget* GoodsText;
	
	UPROPERTY(meta = (BindWidget))
	UVerticalBox* PerkListContainer;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perk")
	TSubclassOf<UGRPerkListWidget> PerkListClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perk")
	TSubclassOf<UGRPerkTooltipWidget> PerkTooltipClass;

	UPROPERTY()
	UGRPerkTooltipWidget* PerkTooltipWidget;
	
	UPROPERTY()
	UGRPerkSubsystem* PerkSubsystem;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perk")
	UDataTable* PerkTable;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perk")
	TArray<FName> PerkCategories = { FName("Battle"), FName("Weapon"), FName("Skill"), FName("SubSkill")};
	
	virtual void NativeConstruct() override;

public:
	UFUNCTION()
	void ShowTooltipForSlot(UGRPerkSlotWidget* PerkSlot);

	UFUNCTION()
	void HideTooltipForSlot(UGRPerkSlotWidget* PerkSlot);

	UFUNCTION()
	void UpdateUIOnClicked(UGRPerkSlotWidget* PerkSlot);

	void ShowTooltip(UGRPerkSlotWidget* PerkSlot);
	
	void HideTooltip();

	void UpdateTooltip(UGRPerkSlotWidget* PerkSlot);

	void UpdateGoodsText();

private:
	void CreatePerkList();
};
