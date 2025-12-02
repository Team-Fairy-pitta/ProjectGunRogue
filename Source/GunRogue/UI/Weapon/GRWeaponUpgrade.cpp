#include "GRWeaponUpgrade.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Player/Battle/GRBattlePlayerController.h"
#include "Player/GRPlayerState.h"
#include "Weapon/GRWeaponDefinition.h"
#include "Components/Image.h"
#include "Components/Border.h"
#include "Components/ScrollBox.h"
#include "UI/Weapon/GROptionSlot.h"

void UGRWeaponUpgrade::Init(int32 InWeaponSlotIndex)
{
	WeaponSlotIndex = InWeaponSlotIndex;
}

void UGRWeaponUpgrade::BlindWeapon(bool bInBlind)
{
	if (!BlindBorder)
	{
		return;
	}
	if (bInBlind)
	{
		BlindBorder->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		BlindBorder->SetVisibility(ESlateVisibility::Collapsed);
	}
	
}

void UGRWeaponUpgrade::SettingWeapon()
{
	if (!GetWorld())
	{
		return;
	}

	if (APlayerController* PC = GetOwningPlayer())
	{
		if (AGRPlayerState* GRPS = PC->GetPlayerState<AGRPlayerState>())
		{

			const UGRWeaponDefinition* WeaponDefinition = GRPS->GetWeaponDefinitionInSlot(WeaponSlotIndex);
			const FGRWeaponInstance* WeaponInstance = GRPS->GetWeaponInstanceInSlot(WeaponSlotIndex);

			if (!WeaponDefinition)
			{
				UE_LOG(LogTemp, Warning, TEXT("WeaponDefinition is invalid"));
				BlindWeapon(true);
				return;
			}

			if (!WeaponInstance)
			{
				UE_LOG(LogTemp, Warning, TEXT("WeaponInstance is invalid"));
				BlindWeapon(true);
				return;
			}

			BlindWeapon(false);

			FText WeaponName = WeaponDefinition->WeaponName;
			UTexture2D* WeaponIcon = WeaponDefinition->WeaponIcon;
			int32 WeaponLevel = WeaponInstance->GetLevel();
			float WeaponDamage = WeaponInstance->GetDamage();
			float WeaponWeakpoint = 150.f;
			float WeaponLaunchspeed = 500;
			float WeaponMagazine = 300;
			FText WeaponExplain = WeaponDefinition->WeaponDescription;

			WeaponNameUpdate(WeaponName);
			WeaponImageUpdate(WeaponIcon);
			WeaponLevelUpdate(WeaponLevel);
			WeaponDamageUpdate(WeaponDamage);
			WeaponWeakpointUpdate(WeaponWeakpoint);
			WeaponLaunchspeedUpdate(WeaponLaunchspeed);
			WeaponMagazineUpdate(WeaponMagazine);
			WeaponExplainUpdate(WeaponExplain);
			WeaponOptionUpdate();
		}
	}
}

void UGRWeaponUpgrade::NativeConstruct()
{
	Super::NativeConstruct();

	if (UpgradeButton)
	{
		UpgradeButton->OnClicked.AddUniqueDynamic(this, &UGRWeaponUpgrade::UpGrade);
	}

	if (RerollButton)
	{
		RerollButton->OnClicked.AddUniqueDynamic(this, &UGRWeaponUpgrade::RerollOption);
	}

	SettingWeapon();

	if (AGRPlayerState* GRPS = GetOwningPlayerState<AGRPlayerState>())
	{
		if (!WeaponDataUpdateHandle.IsValid())
		{
			WeaponDataUpdateHandle = GRPS->OnWeaponDataUpdata.AddUObject(this, &UGRWeaponUpgrade::SettingWeapon);
		}
	}

	CurrentOptionSlotIndex = -1;
	WeaponOptionUpdate();
}

void UGRWeaponUpgrade::NativeDestruct()
{
	if (UpgradeButton)
	{
		UpgradeButton->OnClicked.RemoveDynamic(this, &UGRWeaponUpgrade::UpGrade);
	}

	if (RerollButton)
	{
		RerollButton->OnClicked.RemoveDynamic(this, &UGRWeaponUpgrade::RerollOption);
	}

	if (AGRPlayerState* GRPS = GetOwningPlayerState<AGRPlayerState>())
	{
		if (WeaponDataUpdateHandle.IsValid())
		{
			GRPS->OnWeaponDataUpdata.Remove(WeaponDataUpdateHandle);
			WeaponDataUpdateHandle.Reset();
		}
	}

	Super::NativeDestruct();
}

void UGRWeaponUpgrade::UpGrade()
{
	if (!GetWorld())
	{
		return;
	}

	if (APlayerController* PC = GetOwningPlayer())
	{
		if (AGRPlayerState* GRPS = PC->GetPlayerState<AGRPlayerState>())
		{
			GRPS->UpgradeWeapon(WeaponSlotIndex);
		}
	}
}

void UGRWeaponUpgrade::RerollOption()
{
	UE_LOG(LogTemp, Warning, TEXT("RerollOption"));

	if (!GetWorld())
	{
		return;
	}

	if (CurrentOptionSlotIndex == -1)
	{
		return;
	}

	if (APlayerController* PC = GetOwningPlayer())
	{
		if (AGRPlayerState* GRPS = PC->GetPlayerState<AGRPlayerState>())
		{
			GRPS->RerollOptionWeapon(WeaponSlotIndex, CurrentOptionSlotIndex);
		}
	}

	WeaponOptionUpdate();
}

void UGRWeaponUpgrade::AllRerollOption()
{
	UE_LOG(LogTemp, Warning, TEXT("AllRerollOption"));

	if (!GetWorld())
	{
		return;
	}

	if (APlayerController* PC = GetOwningPlayer())
	{
		if (AGRPlayerState* GRPS = PC->GetPlayerState<AGRPlayerState>())
		{
			GRPS->AllRerollOptionWeapon(WeaponSlotIndex);
		}
	}

	WeaponOptionUpdate();

}

void UGRWeaponUpgrade::WeaponNameUpdate(FText InWeaponName)
{
	if (!WeaponNameText)
	{
		return;
	}

	WeaponNameText->SetText(InWeaponName);
}

void UGRWeaponUpgrade::WeaponImageUpdate(UTexture2D* InWeaponImage)
{
	if (!WeaponIconIamge)
	{
		return;
	}

	WeaponIconIamge->SetBrushFromTexture(InWeaponImage);
}

void UGRWeaponUpgrade::WeaponLevelUpdate(int32 InWeaponLevel)
{
	if (!WeaponLevelText)
	{
		return;
	}

	WeaponLevelText->SetText(FText::Format(
		FText::FromString(TEXT("+{0}")),
		FText::AsNumber(InWeaponLevel)
	));
}

void UGRWeaponUpgrade::WeaponDamageUpdate(float InWeaponDamage)
{
	if (!WeaponDamageText)
	{
		return;
	}

	WeaponDamageText->SetText(FText::AsNumber(InWeaponDamage));
}

void UGRWeaponUpgrade::WeaponWeakpointUpdate(float InWeakpoint)
{
	if (!WeaponWeakpointText)
	{
		return;
	}

	WeaponWeakpointText->SetText(FText::Format(
		FText::FromString(TEXT("{0}%")),
		FText::AsNumber(InWeakpoint)
	));
}

void UGRWeaponUpgrade::WeaponLaunchspeedUpdate(float InWeaponLaunchspeed)
{
	if (!WeaponLaunchspeedText)
	{
		return;
	}

	WeaponLaunchspeedText->SetText(FText::AsNumber(InWeaponLaunchspeed));
}

void UGRWeaponUpgrade::WeaponMagazineUpdate(float InWeaponMagazine)
{
	if (!WeaponMagazineText)
	{
		return;
	}

	WeaponMagazineText->SetText(FText::AsNumber(InWeaponMagazine));
}

void UGRWeaponUpgrade::WeaponExplainUpdate(FText InWeaponExplain)
{
	if (!WeaponExplainText)
	{
		return;
	}

	WeaponExplainText->SetText(InWeaponExplain);
}

void UGRWeaponUpgrade::WeaponOptionUpdate()
{
	if (!GetWorld() || !OptionSlotClass)
	{
		return;
	}

	OptionWidgets.Empty();
	OptionList->ClearChildren();

	AGRPlayerState* GRPS = GetOwningPlayerState<AGRPlayerState>();
	if (!GRPS)
	{
		return;
	}

	const FGRWeaponInstance* WeaponInstance = GRPS->GetWeaponInstanceInSlot(WeaponSlotIndex);
	if (!WeaponInstance)
	{
		return;
	}

	const TArray<FWeaponOption>& Options = WeaponInstance->Options;  

	for (int32 i = 0; i < Options.Num(); i++)
	{
		UGROptionSlot* Entry = CreateWidget<UGROptionSlot>(GetWorld(), OptionSlotClass);

		Entry->InitSlot(i, Options[i]);
		Entry->OnOptionClicked.BindUObject(this, &ThisClass::OnOptionSelected);

		OptionList->AddChild(Entry);
		OptionWidgets.Add(Entry);

		bool bSelected = (i == CurrentOptionSlotIndex);
		OptionWidgets[i]->SetSelected(bSelected);
	}
}

void UGRWeaponUpgrade::OnOptionSelected(int32 InOptionSlotIndex)
{
	if (CurrentOptionSlotIndex == InOptionSlotIndex)
	{
		OptionWidgets[InOptionSlotIndex]->SetSelected(false);
		CurrentOptionSlotIndex = -1;
		return;
	}

		
	CurrentOptionSlotIndex = InOptionSlotIndex;

	for (int32 i = 0; i < OptionWidgets.Num(); ++i)
	{
		bool bSelected = (i == CurrentOptionSlotIndex);
		OptionWidgets[i]->SetSelected(bSelected);
	}
}
