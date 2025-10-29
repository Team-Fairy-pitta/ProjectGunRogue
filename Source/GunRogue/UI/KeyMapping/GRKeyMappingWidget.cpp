#include "UI/KeyMapping/GRKeyMappingWidget.h"
#include "UI/KeyMapping/GRKeyMappingSlot.h"
#include "Components/Scrollbox.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedInputLibrary.h"
#include "UserSettings/EnhancedInputUserSettings.h"

void UGRKeyMappingWidget::InitKeyMappings(TObjectPtr<UEnhancedInputUserSettings> UserSetting, TMap<FName, FPlayerKeyMapping>& Mappings)
{
	if (!ScrollBox)
	{
		return;
	}

	CachedUserSetting = UserSetting;

	for (auto& MappingPair : Mappings)
	{
		const FName& ActionName = MappingPair.Key;
		const FPlayerKeyMapping& Mapping = MappingPair.Value;
		const FName& KeyName = Mapping.GetCurrentKey().GetFName();

		UGRKeyMappingSlot* NewSlot = CreateWidget<UGRKeyMappingSlot>(GetOwningPlayer(), SlotClass);

		if (NewSlot)
		{
			NewSlot->Init(ActionName, KeyName, this);
			ScrollBox->AddChild(NewSlot);
		}

	}
}

void UGRKeyMappingWidget::ApplyKeyMappings(ULocalPlayer* InLocalPlayer, UInputMappingContext* MappingContext)
{
	UEnhancedInputLocalPlayerSubsystem* Subsystem = InLocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	if (!MappingContext)
	{
		return;
	}

	int32 MappingPriority = 0;
	Subsystem->AddMappingContext(MappingContext, MappingPriority);
	CachedUserSetting->RegisterInputMappingContext(MappingContext);
	UEnhancedInputLibrary::RequestRebuildControlMappingsUsingContext(MappingContext, true);
}

void UGRKeyMappingWidget::ClearWidgets()
{
	if (!ScrollBox)
	{
		return;
	}

	ScrollBox->ClearChildren();
}

void UGRKeyMappingWidget::StartChange(UGRKeyMappingSlot* Current)
{
	isChanging = true;
	CurrentChangingSlot = Current;
}

void UGRKeyMappingWidget::ChangeKeyMapping(const FName& InActionName, const FKey& NewKey)
{
	check(CachedUserSetting);

	FMapPlayerKeyArgs Args = {};
	Args.MappingName = InActionName;
	Args.Slot = EPlayerMappableKeySlot::First;
	Args.NewKey = NewKey;

	FGameplayTagContainer FailureReason;

	CachedUserSetting->MapPlayerKey(Args, FailureReason);
	CachedUserSetting->ApplySettings();
	CachedUserSetting->SaveSettings();
}

void UGRKeyMappingWidget::EndChange()
{
	isChanging = false;
	if (CurrentChangingSlot)
	{
		CurrentChangingSlot->UnfocusChangeButton();
	}
	CurrentChangingSlot = nullptr;
}


bool UGRKeyMappingWidget::IsChanging()
{
	return isChanging;
}
