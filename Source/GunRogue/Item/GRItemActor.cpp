#include "Item/GRItemActor.h"
#include "Item/GRItemDefinition.h"
#include "Components/StaticMeshComponent.h"
#include "Components/BoxComponent.h"
#include "Character/GRCharacter.h"
#include "Player/GRPlayerState.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"

void FGRItemHandle::EquipItem(UGRAbilitySystemComponent* ASC, UGRItemDefinition* ItemDefinition)
{
	if (!IsValid(ASC))
	{
		UE_LOG(LogTemp, Error, TEXT("ASC (UGRAbilitySystemComponent) is INVALID"));
		return;
	}
	if (!IsValid(ItemDefinition))
	{
		UE_LOG(LogTemp, Error, TEXT("ItemDefinition (UGRItemDefinition) is INVALID"));
		return;
	}
	if (!IsValid(ItemDefinition->AbilitySet))
	{
		UE_LOG(LogTemp, Error, TEXT("ItemDefinition->AbilitySet (UGRAbilitySet) is INVALID"));
		return;
	}

	CachedASC = ASC;
	ItemDefinition->AbilitySet->GiveToAbilitySystem(CachedASC, &GrantedHandles);
}

void FGRItemHandle::UnequipItem()
{
	if (!IsValid(CachedASC))
	{
		UE_LOG(LogTemp, Error, TEXT("CachedASC (UGRAbilitySystemComponent) is INVALID"));
		return;
	}

	GrantedHandles.TakeFromAbilitySystem(CachedASC);
}


AGRItemActor::AGRItemActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	SetRootComponent(StaticMeshComponent);

	InteractionBoxComponent = CreateDefaultSubobject<UBoxComponent>(TEXT("InteractionBoxComponent"));
	InteractionBoxComponent->SetupAttachment(StaticMeshComponent);
}

void AGRItemActor::BeginPlay()
{
	Super::BeginPlay();
	
}

TArray<TObjectPtr<UStaticMeshComponent>> AGRItemActor::GetMeshComponents()
{
	TArray<TObjectPtr<UStaticMeshComponent>> MeshComponents;
	MeshComponents.Add(StaticMeshComponent);
	return MeshComponents;
}

void AGRItemActor::InteractWith(AActor* OtherActor)
{
	if (!HasAuthority())
	{
		UE_LOG(LogTemp, Error, TEXT("InteractWith() REQUIRES authority"));
		return;
	}

	AGRCharacter* GRCharacter = Cast<AGRCharacter>(OtherActor);
	if (!IsValid(GRCharacter))
	{
		UE_LOG(LogTemp, Error, TEXT("GRCharacter (AGRCharacter) is INVALID"));
		return;
	}

	AGRPlayerState* GRPlayerState = GRCharacter->GetPlayerState<AGRPlayerState>();
	if (!IsValid(GRPlayerState))
	{
		UE_LOG(LogTemp, Error, TEXT("GRPlayerState (AGRPlayerState) is INVALID"));
		return;
	}

	if (!ItemDefinition)
	{
		UE_LOG(LogTemp, Error, TEXT("ItemDefinition (UGRItemDefinition) is INVALID"));
		return;
	}

	GRPlayerState->EquipItem(ItemDefinition);
	Destroy();
}
