#include "GRWeaponUpgrade.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Player/Battle/GRBattlePlayerController.h"
#include "Player/GRPlayerState.h"
#include "Weapon/GRWeaponDefinition.h"
#include "Components/Image.h"


void UGRWeaponUpgrade::ResetWeapon()
{
	// 텍스트는 모두 빈 값으로
	const FText EmptyText = FText::GetEmpty();

	// 이미지 없음
	UTexture2D* EmptyIcon = nullptr;

	// 수치 값 기본 0
	const int32 DefaultLevel = 0;
	const float DefaultFloat = 0.f;

	WeaponNameUpdate(EmptyText);
	WeaponImageUpdate(EmptyIcon);
	WeaponLevelUpdate(DefaultLevel);
	WeaponDamageUpdate(DefaultFloat);
	WeaponWeakpointUpdate(DefaultFloat);
	WeaponLaunchspeedUpdate(DefaultFloat);
	WeaponMagazineUpdate(DefaultFloat);
	WeaponExplainUpdate(EmptyText);
}

void UGRWeaponUpgrade::SettingWeapon()
{
	if (!GetWorld())
	{
		return;
	}

	if (APlayerController* PC = GetOwningPlayer())
	{
		if (AGRPlayerState* PS = PC->GetPlayerState<AGRPlayerState>())
		{

			const UGRWeaponDefinition* WeaponDefinition = PS->GetWeaponDefinitionInSlot(0);
			const FGRWeaponInstance* WeaponInstance = PS->GetWeaponInstanceInSlot(0);

			if (!WeaponDefinition)
			{
				UE_LOG(LogTemp, Error, TEXT("WeaponDefinition is invalid"));
				ResetWeapon();
				return;
			}

			if (!WeaponInstance)
			{
				UE_LOG(LogTemp, Error, TEXT("WeaponInstance is invalid"));
				ResetWeapon();
				return;
			}

			FText WeaponName = WeaponDefinition->WeaponName;
			UTexture2D* WeaponIcon = WeaponDefinition->WeaponIcon;
			int32 WeaponLevel = WeaponInstance->CurrentLevel;
			float WeaponDamage = WeaponInstance->CurrentDamage;
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
			//WeaponOptionUpdate();
		}
	}
}

void UGRWeaponUpgrade::NativeConstruct()
{
	Super::NativeConstruct();
	SetWidgetFocusable();

	if (UpgradeButton)
	{
		UpgradeButton->OnClicked.AddDynamic(this, &UGRWeaponUpgrade::UpGrade);
	}
	if (RerollButton)
	{
		RerollButton->OnClicked.AddDynamic(this, &UGRWeaponUpgrade::Reroll);
	}

	SettingWeapon();

	if (AGRPlayerState* PS = GetOwningPlayerState<AGRPlayerState>())
	{
		PS->OnWeaponDataChanged.AddUObject(this, &UGRWeaponUpgrade::SettingWeapon);
	}

}

void UGRWeaponUpgrade::NativeDestruct()
{
	if (UpgradeButton)
	{
		UpgradeButton->OnClicked.RemoveDynamic(this, &UGRWeaponUpgrade::UpGrade);
	}
	if (RerollButton)
	{
		RerollButton->OnClicked.RemoveDynamic(this, &UGRWeaponUpgrade::Reroll);
	}

	Super::NativeDestruct();
}

FReply UGRWeaponUpgrade::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	if (InKeyEvent.GetKey() == EKeys::Escape)
	{
		APlayerController* PlayerController = GetOwningPlayer();
		AGRBattlePlayerController* BattlePlayerController = Cast<AGRBattlePlayerController>(PlayerController);

		if (IsValid(BattlePlayerController))
		{
			BattlePlayerController->HideUpgradeConsoleWidget();
		}

		return FReply::Handled();
	}
	return Super::NativeOnKeyDown(InGeometry, InKeyEvent);
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
			GRPS->UpgradeWeapon(0);
		}
	}
}

void UGRWeaponUpgrade::Reroll()
{
	//WeaponOptionUpdate();
}

void UGRWeaponUpgrade::WeaponNameUpdate(FText WeaponName)
{
	if (!WeaponNameText)
	{
		return;
	}

	WeaponNameText->SetText(WeaponName);
}

void UGRWeaponUpgrade::WeaponImageUpdate(UTexture2D* Image)
{
	if (!WeaponIconIamge)
	{
		return;
	}

	WeaponIconIamge->SetBrushFromTexture(Image);
}

void UGRWeaponUpgrade::WeaponLevelUpdate(int32 Level)
{
	if (!WeaponLevelText)
	{
		return;
	}

	WeaponLevelText->SetText(FText::Format(
		FText::FromString(TEXT("+{0}")),
		FText::AsNumber(Level)
	));
}

void UGRWeaponUpgrade::WeaponDamageUpdate(float Damage)
{
	if (!WeaponDamageText)
	{
		return;
	}

	WeaponDamageText->SetText(FText::AsNumber(Damage));
}

void UGRWeaponUpgrade::WeaponWeakpointUpdate(float Weakpoint)
{
	if (!WeaponWeakpointText)
	{
		return;
	}

	WeaponWeakpointText->SetText(FText::Format(
		FText::FromString(TEXT("{0}%")),
		FText::AsNumber(Weakpoint)
	));
}

void UGRWeaponUpgrade::WeaponLaunchspeedUpdate(float Launchspeed)
{
	if (!WeaponLaunchspeedText)
	{
		return;
	}

	WeaponLaunchspeedText->SetText(FText::AsNumber(Launchspeed));
}

void UGRWeaponUpgrade::WeaponMagazineUpdate(float Magazine)
{
	if (!WeaponMagazineText)
	{
		return;
	}

	WeaponMagazineText->SetText(FText::AsNumber(Magazine));
}

void UGRWeaponUpgrade::WeaponExplainUpdate(FText WeaponExplain)
{
	if (!WeaponExplainText)
	{
		return;
	}

	WeaponExplainText->SetText(WeaponExplain);
}

void UGRWeaponUpgrade::SetWidgetFocusable()
{
	bIsFocusable = true;
}

//void UGRWeaponUpgrade::WeaponOptionUpdate()
//{
//}

