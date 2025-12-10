// Fill out your copyright notice in the Description page of Project Settings.


#include "TestUI/TestUIDamageIndicator.h"

#include "UI/Damage/GRDamageIndicator.h"

void ATestUIDamageIndicator::ShowDamageIndicator(float Damage, AActor* DamagedActor)
{
	if (!DamageIndicatorWidgetClass)
	{
		return;
	}

	DamageIndicatorWidgetInstance = CreateWidget<UGRDamageIndicator>(this, DamageIndicatorWidgetClass);
	if (!DamageIndicatorWidgetInstance)
	{
		return;
	}

	DamageIndicatorWidgetInstance->SetData(Damage, DamagedActor);
	DamageIndicatorWidgetInstance->AddToViewport();
}
