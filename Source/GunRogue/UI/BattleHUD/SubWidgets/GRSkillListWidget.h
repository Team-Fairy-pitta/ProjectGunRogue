// GRSkillListWidget.h

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GRSkillListWidget.generated.h"

class UGRSkillSlotWidget;
/**
 * 
 */
UCLASS()
class GUNROGUE_API UGRSkillListWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UGRSkillSlotWidget* GetFirstSkillSlot() const { return FirstSkillSlot; }
	UGRSkillSlotWidget* GetSecondSkillSlot() const { return SecondSkillSlot; }
	UGRSkillSlotWidget* GetThirdSkillSlot() const { return ThirdSkillSlot; }

protected:
	UPROPERTY(meta = (BindWidget))
	UGRSkillSlotWidget* FirstSkillSlot;

	UPROPERTY(meta = (BindWidget))
	UGRSkillSlotWidget* SecondSkillSlot;

	UPROPERTY(meta = (BindWidget))
	UGRSkillSlotWidget* ThirdSkillSlot;
};
