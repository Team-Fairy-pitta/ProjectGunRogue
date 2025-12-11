// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/GameSetting/GRGameUserSettings.h"

void UGRGameUserSettings::SetMasterVolume(float InVolume)
{
	if (MasterVolume != InVolume)
	{
		MasterVolume = InVolume;
	}

	SaveSettings();
}

void UGRGameUserSettings::SetBgmVolume(float InVolume)
{
	if (BgmVolume != InVolume)
	{
		BgmVolume = InVolume;
	}

	SaveSettings();
}

void UGRGameUserSettings::SetSfxVolume(float InVolume)
{
	if (SfxVolume != InVolume)
	{
		SfxVolume = InVolume;
	}

	SaveSettings();
}

float UGRGameUserSettings::GetMasterVolume() const
{
	return MasterVolume;
}

float UGRGameUserSettings::GetBgmVolume() const
{
	return BgmVolume;
}

float UGRGameUserSettings::GetSfxVolume() const
{
	return SfxVolume;
}
