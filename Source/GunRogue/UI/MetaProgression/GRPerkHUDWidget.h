// GRPerkHUDWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRPerkHUDWidget.generated.h"

class UGRGoodsTextWidget;
class UVerticalBox;
class UGRPerkListWidget;
class UGRPerkSlotWidget;
class UGRPerkTooltipWidget;
class UButton;
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
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Perk")
	TArray<FName> PerkCategories = { FName("Battle"), FName("Weapon"), FName("Skill"), FName("SubSkill")};

	UPROPERTY(meta = (BindWidget))
	UButton* ExitPerkButton;
	
	virtual void NativeConstruct() override;

	virtual void NativeDestruct() override;

public:
	UFUNCTION()
	void ShowTooltipForSlot(UGRPerkSlotWidget* PerkSlot);

	UFUNCTION()
	void HideTooltipForSlot(UGRPerkSlotWidget* PerkSlot);

	UFUNCTION()
	void UpdateHUDAndTooltip(UGRPerkSlotWidget* PerkSlot);

	void ShowTooltip(UGRPerkSlotWidget* PerkSlot);
	
	void HideTooltip();

	void UpdateTooltip(UGRPerkSlotWidget* PerkSlot);
	
	void UpdateGoodsText();

private:
	void CreatePerkList();

	UFUNCTION()
	void OnExitPerkClicked();
};
