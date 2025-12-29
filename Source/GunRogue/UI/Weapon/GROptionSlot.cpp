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


			for(const FOptionItem& OptionItem : InOptionData.OptionItems)
			{
				const FGameplayTag& Tag = OptionItem.OptionTag;

				float TargetValue = 0.0f;
				if (InOptionData.bIsPercentValue)
				{
					TargetValue = OptionItem.Value * 100;
					if (InOptionData.bIsAdditivePercent)
					{
						TargetValue -= 100.0f; // [Note] 공격력이 130% 된다. -> 공격력이 30% 증가
					}
				}
				else
				{
					TargetValue = OptionItem.Value;
				}

				TargetValue *= 10;
				TargetValue = FMath::RoundToFloat(TargetValue);
				TargetValue /= 10;

				FString TagString = Tag.ToString();
				FString ValueString = FString::SanitizeFloat(TargetValue);
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
