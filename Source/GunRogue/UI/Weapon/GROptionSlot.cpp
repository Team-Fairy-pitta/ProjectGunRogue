#include "UI/Weapon/GROptionSlot.h"

#include "Components/Button.h"
#include "Components/Border.h"

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

void UGROptionSlot::SetSelected(bool bSelected)
{
	SelectBorder->SetBrushColor(bSelected ? FLinearColor::Yellow : FLinearColor::Black);
}
