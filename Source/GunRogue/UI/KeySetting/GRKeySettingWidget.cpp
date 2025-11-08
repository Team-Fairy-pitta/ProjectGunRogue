#include "UI/KeySetting/GRKeySettingWidget.h"
#include "UI/KeySetting/GRKeySettingCategory.h"
#include "Player/GRPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "Components/Button.h"
#include "Components/VerticalBox.h"

void UGRKeySettingWidget::NativeConstruct()
{
	if (!ResetButton)
	{
		UE_LOG(LogTemp, Error, TEXT("ResetButton is INVALID."));
		return;
	}
	ResetButton->OnClicked.AddDynamic(this, &ThisClass::OnResetButtonClicked);
}

void UGRKeySettingWidget::InitKeyMappings()
{
	CachedUserSetting = LoadUserSetting();
	if (!CachedUserSetting)
	{
		UE_LOG(LogTemp, Error, TEXT("CachedUserSetting (UEnhancedInputUserSettings) is INVALID."));
		return;
	}

	TMap<FString, FKeyMappingCategory> Categories;
	GetKeyMappings(Categories);
	AddKeyCategories(Categories);
}

void UGRKeySettingWidget::UpdateMappings(const FName& InMappingName, const FKey& NewKey)
{
	FMapPlayerKeyArgs Args = {};
	Args.MappingName = InMappingName;
	Args.Slot = EPlayerMappableKeySlot::First;
	Args.NewKey = NewKey;

	FGameplayTagContainer FailureReason;

	CachedUserSetting->MapPlayerKey(Args, FailureReason);
	CachedUserSetting->ApplySettings();
	CachedUserSetting->SaveSettings();
}

UEnhancedInputUserSettings* UGRKeySettingWidget::LoadUserSetting()
{
	AGRPlayerController* GRPlayerController = GetOwningPlayer<AGRPlayerController>();
	if (!IsValid(GRPlayerController))
	{
		UE_LOG(LogTemp, Error, TEXT("Widget Owner(Player Controller) is INVALID."));
		return nullptr;
	}

	ULocalPlayer* LocalPlayer = GRPlayerController->GetLocalPlayer();
	if (!LocalPlayer)
	{
		UE_LOG(LogTemp, Error, TEXT("LocalPlayer (PlayerController->GetLocalPlayer()) is INVALID."));
		return nullptr;
	}

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	if (!Subsystem)
	{
		UE_LOG(LogTemp, Error, TEXT("Subsystem (UEnhancedInputLocalPlayerSubsystem) is INVALID."));
		return nullptr;
	}

	UEnhancedInputUserSettings* Settings = Subsystem->GetUserSettings();
	if (!Settings)
	{
		UE_LOG(LogTemp, Error, TEXT("Settings (UEnhancedInputUserSettings::LoadOrCreateSettings()) is INVALID."));
		return nullptr;
	}

	for (UInputMappingContext* MappableContext : GRPlayerController->MappableMappingContexts)
	{
		Settings->RegisterInputMappingContext(MappableContext);
	}

	return Settings;
}

void UGRKeySettingWidget::GetKeyMappings(TMap<FString, FKeyMappingCategory>& OutMappings)
{
	if (!CachedUserSetting)
	{
		UE_LOG(LogTemp, Error, TEXT("CachedUserSetting (UEnhancedInputUserSettings) is INVALID."));
		return;
	}

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
				const FText& CategoryText = KeyMapping.GetDisplayCategory();
				FString CategoryString = CategoryText.ToString();
				if (!OutMappings.Contains(CategoryString))
				{
					OutMappings.Add(CategoryString);
				}
				OutMappings[CategoryString].CategoryText = CategoryText;
				OutMappings[CategoryString].Mappings.Add(KeyMapping);

				// NOTE: 현재 키 세팅은 1개만 지원함
				break;
			}
		}
	}
}

void UGRKeySettingWidget::AddKeyCategories(TMap<FString, FKeyMappingCategory>& InCategories)
{
	if (!CategoryWidgetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("CategoryWidgetClass (TSubclassOf<UGRKeySettingCategory>) is INVALID."));
		return;
	}

	if (!CategoriesVerticalBox)
	{
		UE_LOG(LogTemp, Error, TEXT("CategoriesVerticalBox (UVerticalBox) is INVALID."));
		return;
	}

	CategoriesVerticalBox->ClearChildren();
	
	for (const auto& Pair : InCategories)
	{
		const FText& CategoryText = Pair.Value.CategoryText;
		const TArray<FPlayerKeyMapping>& Mappings = Pair.Value.Mappings;

		UGRKeySettingCategory* NewCategory = CreateWidget<UGRKeySettingCategory>(GetOwningPlayer(), CategoryWidgetClass);
		if (NewCategory)
		{
			NewCategory->SetCategoryText(CategoryText);
			NewCategory->AddMappings(Mappings, this /*ParentWidget*/);
			CategoriesVerticalBox->AddChild(NewCategory);
		}
	}
}

void UGRKeySettingWidget::OnResetButtonClicked()
{
	if (!CachedUserSetting)
	{
		UE_LOG(LogTemp, Error, TEXT("CachedUserSetting (UEnhancedInputUserSettings) is INVALID."));
		return;
	}

	TMap<FString, FKeyMappingCategory> Categories;
	GetKeyMappings(Categories);

	for (const auto& Pair : Categories)
	{
		const TArray<FPlayerKeyMapping>& Mappings = Pair.Value.Mappings;

		for (const FPlayerKeyMapping& Mapping : Mappings)
		{
			FMapPlayerKeyArgs Args = {};
			Args.MappingName = Mapping.GetMappingName();
			Args.Slot = EPlayerMappableKeySlot::First;
			Args.NewKey = Mapping.GetDefaultKey();

			FGameplayTagContainer FailureReason;
			CachedUserSetting->MapPlayerKey(Args, FailureReason);
		}
	}
	CachedUserSetting->ApplySettings();
	CachedUserSetting->SaveSettings();

	AddKeyCategories(Categories);
}
