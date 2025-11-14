#include "Player/GRPlayerState.h"
#include "Player/GRPlayerController.h"
#include "Character/GRCharacter.h"
#include "Character/GRPawnData.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/GRAbilitySet.h"

AGRPlayerState::AGRPlayerState()
{
	AbilitySystemComponent = CreateDefaultSubobject<UGRAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// AbilitySystem 네트워크 관련: needs to be updated at a high frequency.
	SetNetUpdateFrequency(100.0f);

	OnPawnSet.AddDynamic(this, &ThisClass::OnPawnSetted);
}

void AGRPlayerState::BeginPlay()
{
}

AGRPlayerController* AGRPlayerState::GetGRPlayerController() const
{
	return Cast<AGRPlayerController>(GetOwner());
}

AGRCharacter* AGRPlayerState::GetGRCharacter() const
{
	return Cast<AGRCharacter>(GetPawn());
}

UGRAbilitySystemComponent* AGRPlayerState::GetGRAbilitySystemComponent()
{
	return Cast<UGRAbilitySystemComponent>(AbilitySystemComponent);
}

UAbilitySystemComponent* AGRPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AGRPlayerState::EquipItem(UGRItemDefinition* ItemDefinition)
{
	FGRItemHandle& NewItemHandle = ItemHandles.AddDefaulted_GetRef();
	NewItemHandle.EquipItem(AbilitySystemComponent, ItemDefinition);
}

void AGRPlayerState::UnequipItem(int32 ItemIndex)
{
	if (!ItemHandles.IsValidIndex(ItemIndex))
	{
		return;
	}

	FGRItemHandle& ItemHandle = ItemHandles[ItemIndex];
	ItemHandle.UnequipItem();
	ItemHandles.RemoveAt(ItemIndex);
}

void AGRPlayerState::OnPawnSetted(APlayerState* Player, APawn* NewPawn, APawn* OldPawn)
{
	InitAbilitySystemComponent();
}

void AGRPlayerState::InitAbilitySystemComponent()
{
	AGRCharacter* GRCharacter = GetGRCharacter();
	if (!IsValid(GRCharacter))
	{
		return;
	}

	const UGRPawnData* PwanData = GRCharacter->GetPawnData();
	if (!PwanData)
	{
		return;
	}

	if (!AbilitySystemComponent)
	{
		return;
	}

	AbilitySystemComponent->InitAbilityActorInfo(this /*Owner*/, GRCharacter /*Avatar*/);

	for (UGRAbilitySet* AbilitySet : PwanData->AbilitySets)
	{
		AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, &GrantedHandles);
	}
}
