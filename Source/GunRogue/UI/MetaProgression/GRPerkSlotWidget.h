// GRPerkSlotWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRPerkSlotWidget.generated.h"

class UButton;
class UImage;
class UProgressBar;
class UGRPerkSubsystem;
/**
 * 
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPerkSlotHovered, UGRPerkSlotWidget*, PerkSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPerkSlotClicked, UGRPerkSlotWidget*, PerkSlot);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPerkSlotUnhovered, UGRPerkSlotWidget*, PerkSlot);

UCLASS()
class GUNROGUE_API UGRPerkSlotWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintAssignable)
	FOnPerkSlotHovered OnPerkSlotHovered;

	UPROPERTY(BlueprintAssignable)
	FOnPerkSlotClicked OnPerkSlotClicked;

	UPROPERTY(BlueprintAssignable)
	FOnPerkSlotUnhovered OnPerkSlotUnhovered;
	
	FName GetPerkIDInSlot() const { return PerkID; }
	
protected:
	UPROPERTY(meta = (BindWidget))
	UButton* PerkSlotButton;

	UPROPERTY(meta = (BindWidget))
	UImage* PerkSlotIcon;

	UPROPERTY(meta = (BindWidget))
	UProgressBar* PerkSlotBar;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perk")
	FName PerkID;

	UPROPERTY()
	UGRPerkSubsystem* PerkSubsystem;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Perk")
	UDataTable* PerkTable;
	
	virtual void NativeConstruct() override;

public:
	void SetupSlot(FName InPerkID, UGRPerkSubsystem* InSubsystem);

	void SetPerkSlotIcon(UTexture2D* NewIcon);

	void SetPerkSlotBar(int32 CurrentLevel, int32 MaxLevel);

private:
	UFUNCTION()
	void OnPerkClicked();

	UFUNCTION()
	void OnPerkHovered();

	UFUNCTION()
	void OnPerkUnhovered();

	void UpdatePerkUI();
};
