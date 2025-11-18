#include "GRWeaponUpgrade.h"

#include "Components/Button.h"


void UGRWeaponUpgrade::NativeConstruct()
{
	Super::NativeConstruct();
	if (UpgradeButton)
	{
		UpgradeButton->OnClicked.AddDynamic(this, &UGRWeaponUpgrade::HandleClicked);
	}
}

void UGRWeaponUpgrade::NativeDestruct()
{
	if (UpgradeButton)
	{
		UpgradeButton->OnClicked.RemoveDynamic(this, &UGRWeaponUpgrade::HandleClicked);
	}
	Super::NativeDestruct();
}

void UGRWeaponUpgrade::HandleClicked()
{
	
}


