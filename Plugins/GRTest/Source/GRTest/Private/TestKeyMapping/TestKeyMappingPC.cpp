#include "TestKeyMapping/TestKeyMappingPC.h"
#include "GunRogue/UI/KeyMapping/GRKeyMappingWidget.h"
#include "EnhancedInputSubsystems.h"
#include "UserSettings/EnhancedInputUserSettings.h"

ATestKeyMappingPC::ATestKeyMappingPC()
{
}

void ATestKeyMappingPC::BeginPlay()
{
	Super::BeginPlay();
}

void ATestKeyMappingPC::InitKeyMappings()
{
	LoadOrCreateUserSettings();
	RegisterInputMappingContext(DefaultMappingContext);

	TMap<FName, FPlayerKeyMapping> Mappings = GetPlayerKeyMappings();

	if (!TestWidgetClassInstance)
	{
		return;
	}

	UWidget* TargetWidget = TestWidgetClassInstance->GetWidgetFromName(FName(TEXT("KeyMappings")));
	UGRKeyMappingWidget* MappingWidget = Cast<UGRKeyMappingWidget>(TargetWidget);
	if (IsValid(MappingWidget))
	{
		MappingWidget->InitKeyMappings(CachedUserSetting, Mappings);
	}
}


void ATestKeyMappingPC::ApplyKeyMappings()
{
	ULocalPlayer* LP = GetLocalPlayer();
	check(LP);

	UWidget* TargetWidget = TestWidgetClassInstance->GetWidgetFromName(FName(TEXT("KeyMappings")));
	UGRKeyMappingWidget* MappingWidget = Cast<UGRKeyMappingWidget>(TargetWidget);
	if (IsValid(MappingWidget))
	{
		MappingWidget->ApplyKeyMappings(LP, DefaultMappingContext);
	}
}


void ATestKeyMappingPC::ClearMappingWidget()
{
	UWidget* TargetWidget = TestWidgetClassInstance->GetWidgetFromName(FName(TEXT("KeyMappings")));
	UGRKeyMappingWidget* MappingWidget = Cast<UGRKeyMappingWidget>(TargetWidget);
	if (IsValid(MappingWidget))
	{
		MappingWidget->ClearWidgets();
	}
}

void ATestKeyMappingPC::LoadOrCreateUserSettings()
{
	ULocalPlayer* LocalPlayer = GetLocalPlayer();
	check(LocalPlayer);
	CachedUserSetting = UEnhancedInputUserSettings::LoadOrCreateSettings(LocalPlayer);
}

void ATestKeyMappingPC::SaveInputUserSettings()
{
	check(CachedUserSetting);
	CachedUserSetting->SaveSettings();
}

TMap<FName, FPlayerKeyMapping> ATestKeyMappingPC::GetPlayerKeyMappings()
{
	check(CachedUserSetting);

	TMap<FName, FPlayerKeyMapping> Results;

	for (auto& ProfilePair : CachedUserSetting->GetAllSavedKeyProfiles())
	{
		const FGameplayTag& ProfileName = ProfilePair.Key;
		const TObjectPtr<UEnhancedPlayerMappableKeyProfile>& Profile = ProfilePair.Value;

		for (auto& RowPair : Profile->GetPlayerMappingRows())
		{
			const FName& RowName = RowPair.Key;
			const FKeyMappingRow& MappingRow = RowPair.Value;

			for (const FPlayerKeyMapping& KeyMapping : MappingRow.Mappings)
			{
				Results.Add(RowName, KeyMapping);
			}
		}
	}

	return Results;
}

void ATestKeyMappingPC::RegisterInputMappingContext(UInputMappingContext* MappingContext)
{
	check(CachedUserSetting);
	if (MappingContext)
	{
		CachedUserSetting->RegisterInputMappingContext(MappingContext);
	}
}
