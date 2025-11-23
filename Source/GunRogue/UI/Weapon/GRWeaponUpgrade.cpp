#include "GRWeaponUpgrade.h"

#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Character/GRCharacter.h"
#include "Weapon/GRWeaponBase.h"


void UGRWeaponUpgrade::SettingWeapon()
{
	if (!GetWorld())
	{
		return;
	}

	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}

	AGRCharacter* Character = Cast<AGRCharacter>(PC->GetPawn());
	if (!Character)
	{
		return;
	}

	AGRWeaponBase* Weapon = Character->GetWeapon();
	if (!Weapon)
	{
		return;
	}

	FString WeaponName = TEXT("Gun");
	int32 WeaponLevel = Weapon->GetLevel();
	float WeaponDamage = Weapon->GetDamage();
	float WeaponWeakpoint = 150.f;
	float WeaponLaunchspeed = 500;
	float WeaponMagazine = 300;
	FString WeaponExplain = TEXT("기본 무기이다.");

	WeaponNameUpdate(WeaponName);
	//WeaponImageUpdate();
	WeaponLevelUpdate(WeaponLevel);
	WeaponDamageUpdate(WeaponDamage);
	WeaponWeakpointUpdate(WeaponWeakpoint);
	WeaponLaunchspeedUpdate(WeaponLaunchspeed);
	WeaponMagazineUpdate(WeaponMagazine);
	WeaponExplainUpdate(WeaponExplain);
	//WeaponOptionUpdate();
}

void UGRWeaponUpgrade::NativeConstruct()
{
	Super::NativeConstruct();
	if (UpgradeButton)
	{
		UpgradeButton->OnClicked.AddDynamic(this, &UGRWeaponUpgrade::UpGrade);
	}
	if (RerollButton)
	{
		RerollButton->OnClicked.AddDynamic(this, &UGRWeaponUpgrade::Reroll);
	}

	SettingWeapon();
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

void UGRWeaponUpgrade::UpGrade()
{
	if (!GetWorld())
	{
		return;
	}

	APlayerController* PC = GetOwningPlayer();
	if (!PC)
	{
		return;
	}

	AGRCharacter* Character = Cast<AGRCharacter>(PC->GetPawn());
	if (!Character)
	{
		return;
	}

	Character->Test_UpgradeWeapon();

	FTimerHandle Handle;
	GetWorld()->GetTimerManager().SetTimer(Handle, this, &UGRWeaponUpgrade::SettingWeapon, 0.1f, false);

	AGRWeaponBase* Weapon = Character->GetWeapon();
	if (!Weapon)
	{
		return;
	}

	int32 Level = Weapon->GetLevel();
	float Damage = Weapon->GetDamage();

	WeaponLevelUpdate(Level);
	WeaponDamageUpdate(Damage);
	//WeaponOptionUpdate();
}

void UGRWeaponUpgrade::Reroll()
{
	//WeaponOptionUpdate();
}

void UGRWeaponUpgrade::WeaponNameUpdate(FString WeaponName)
{
	if (!WeaponNameText)
	{
		return;
	}

	WeaponNameText->SetText(FText::FromString(WeaponName));
}

//void UGRWeaponUpgrade::WeaponImageUpdate()
//{
//}

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

void UGRWeaponUpgrade::WeaponExplainUpdate(FString WeaponExplain)
{
	if (!WeaponExplainText)
	{
		return;
	}

	WeaponExplainText->SetText(FText::FromString(WeaponExplain));
}

//void UGRWeaponUpgrade::WeaponOptionUpdate()
//{
//}



