#include "UI/Weapon/GROptionSlot.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "AbilitySystem/GRGameplayEffect.h"
#include "Weapon/GRWeaponInstance.h"


void UGROptionSlot::NativeConstruct()
{
	Super::NativeConstruct();

	if (SelectButton)
	{
		SelectButton->OnClicked.AddUniqueDynamic(this, &ThisClass::HandleClick);
	}
}

void UGROptionSlot::NativeDestruct()
{
	if (SelectButton)
	{
		SelectButton->OnClicked.RemoveDynamic(this, &ThisClass::HandleClick);
	}

	Super::NativeDestruct();
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
			FString Str = Desc.ToString();


			for(const TPair< FGameplayTag, float>& Pair : InOptionData.Values)
			{
				FString TagString = Pair.Key.ToString();
				FString ValueString = FString::SanitizeFloat(Pair.Value);
				Str = Str.Replace(*FString::Printf(TEXT("{%s}"), *TagString), *ValueString);
			}

			OptionText->SetText(FText::Format(FText::FromString(TEXT("- {0}")),	FText::FromString(Str)));
		}
	}
}

void UGROptionSlot::SetSelected(bool bSelected)
{
	FLinearColor Color = bSelected ? FLinearColor::Red : FLinearColor::White;
	OptionText->SetColorAndOpacity(Color);
}
