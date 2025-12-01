#include "UI/Weapon/GROptionSlot.h"

#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "AbilitySystem/GRGameplayEffect.h"
#include "Weapon/GRWeaponInstance.h"


void UGROptionSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if (SelectButton)
	{
		SelectButton->OnClicked.AddDynamic(this, &ThisClass::HandleClick);
	}
}

void UGROptionSlot::HandleClick()
{
	OnOptionClicked.ExecuteIfBound(OptionSlotIndex);
}

void UGROptionSlot::InitSlot(int32 InSlotIndex, const FWeaponOption& InOptionData)
{
	OptionSlotIndex = InSlotIndex;

	if (InOptionData.EffectClass)
	{
		const UGRGameplayEffect* GRGE = InOptionData.EffectClass->GetDefaultObject<UGRGameplayEffect>();
		if (GRGE)
		{
			FText Desc = GRGE->EffectDescription;

			OptionText->SetText(Desc); 
		}
	}
}

void UGROptionSlot::SetSelected(bool bSelected)
{
	if (SelectBorder)
	{
		SelectBorder->SetBrushColor(bSelected ? FLinearColor::Yellow : FLinearColor::Black);
	}
}
