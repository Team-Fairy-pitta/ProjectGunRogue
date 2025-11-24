#include "Player/GRPlayerState.h"
#include "Player/GRPlayerController.h"
#include "Player/Battle/GRBattlePlayerController.h"
#include "Character/GRCharacter.h"
#include "Character/GRPawnData.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/GRAbilitySet.h"
#include "AbilitySystem/GRGameplayEffect.h"
#include "Net/UnrealNetwork.h"
#include "Item/GRItemActor.h"
#include "Item/GRItemDefinition.h"

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

void AGRPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, ItemHandles);
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

bool AGRPlayerState::HasItem(UGRItemDefinition* ItemDefinition)
{
	return ItemDefinitionSet.Contains(ItemDefinition);
}

void AGRPlayerState::TryEquipItem(UGRItemDefinition* ItemDefinition, AActor* ItemActor)
{
	ServerRPC_EquipItemActor(ItemDefinition, ItemActor);
}

void AGRPlayerState::UnequipItem(int32 ItemIndex)
{
	ServerRPC_UnequipItemActor(ItemIndex);
}

int32 AGRPlayerState::GetItemNum()
{
	return ItemHandles.Num();
}

void AGRPlayerState::ServerRPC_EquipItemActor_Implementation(UGRItemDefinition* ItemDefinition, AActor* ItemActor)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!ItemDefinition)
	{
		return;
	}

	if (HasItem(ItemDefinition))
	{
		return;
	}

	FGRItemHandle& NewItemHandle = ItemHandles.AddDefaulted_GetRef();
	NewItemHandle.EquipItem(AbilitySystemComponent, ItemDefinition);

	ItemDefinitionSet.Add(ItemDefinition);

	ItemActor->Destroy();
	
	OnEquipItem(ItemDefinition);
}

void AGRPlayerState::ServerRPC_UnequipItemActor_Implementation(int32 ItemIndex)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!ItemHandles.IsValidIndex(ItemIndex))
	{
		return;
	}

	APawn* Pawn = GetPawn();
	if (!IsValid(Pawn))
	{
		return;
	}

	UWorld* World = GetWorld();
	if (!IsValid(World))
	{
		return;
	}

	FGRItemHandle& ItemHandle = ItemHandles[ItemIndex];
	ItemHandle.UnequipItem();

	UGRItemDefinition* RemovedItemDefinition = ItemHandle.ItemDefinition;
	ItemDefinitionSet.Remove(RemovedItemDefinition);

	ItemHandles.RemoveAt(ItemIndex);

	FVector DropLocation = Pawn->GetActorLocation();
	FRotator DropRotator = Pawn->GetActorRotation();
	FActorSpawnParameters SpawnParam;
	SpawnParam.Owner = nullptr;

	float DropDistance = 100.0f;
	DropLocation = DropLocation + DropRotator.Vector() * DropDistance;

	AGRItemActor* ItemActor = World->SpawnActor<AGRItemActor>(AGRItemActor::StaticClass(), DropLocation, DropRotator, SpawnParam);
	if (ItemActor)
	{
		PlaceActorOnGround(ItemActor);
		ItemActor->MulticastRPC_InitItem(RemovedItemDefinition);
	}

	OnUnequipItem(RemovedItemDefinition);
}

void AGRPlayerState::OnPawnSetted(APlayerState* Player, APawn* NewPawn, APawn* OldPawn)
{
	if (IsValid(NewPawn))
	{
		InitAbilitySystemComponent();
	}
}

void AGRPlayerState::InitAbilitySystemComponent()
{
	if (bIsAbilitySystemComponentInit)
	{
		UE_LOG(LogTemp, Error, TEXT("AbilitySystemComponent Already Init..."));
		return;
	}

	AGRCharacter* GRCharacter = GetGRCharacter();
	if (!IsValid(GRCharacter))
	{
		return;
	}

	const UGRPawnData* PawnData = GRCharacter->GetPawnData();
	if (!PawnData)
	{
		return;
	}

	if (!AbilitySystemComponent)
	{
		return;
	}

	AbilitySystemComponent->InitAbilityActorInfo(this /*Owner*/, GRCharacter /*Avatar*/);

	for (UGRAbilitySet* AbilitySet : PawnData->AbilitySets)
	{
		AbilitySet->GiveToAbilitySystem(AbilitySystemComponent, &GrantedHandles);
	}

	if (OnAbilitySystemComponentInit.IsBound())
	{
		OnAbilitySystemComponentInit.Broadcast();
	}

	bIsAbilitySystemComponentInit = true;
}

void AGRPlayerState::OnEquipItem(UGRItemDefinition* ItemDefinition)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!ItemDefinition)
	{
		return;
	}

	AGRPlayerController* GRPlayerController = GetGRPlayerController();
	AGRBattlePlayerController* GRBattlePlayerController = Cast<AGRBattlePlayerController>(GRPlayerController);
	if (!IsValid(GRBattlePlayerController))
	{
		return;
	}

	if (!ItemDefinition->AbilitySet)
	{
		return;
	}

	for (const FGRAbilitySet_GameplayEffect& Effect : ItemDefinition->AbilitySet->GetGameplayEffects())
	{
		GRBattlePlayerController->ClientRPC_OnActiveGameplayEffectAdded(Effect.GameplayEffect);
	}
}

void AGRPlayerState::OnUnequipItem(UGRItemDefinition* ItemDefinition)
{
	if (!HasAuthority())
	{
		return;
	}

	if (!ItemDefinition)
	{
		return;
	}

	AGRPlayerController* GRPlayerController = GetGRPlayerController();
	AGRBattlePlayerController* GRBattlePlayerController = Cast<AGRBattlePlayerController>(GRPlayerController);
	if (!IsValid(GRBattlePlayerController))
	{
		return;
	}

	if (!ItemDefinition->AbilitySet)
	{
		return;
	}

	for (const FGRAbilitySet_GameplayEffect& Effect : ItemDefinition->AbilitySet->GetGameplayEffects())
	{
		GRBattlePlayerController->ClientRPC_OnActiveGameplayEffectRemoved(Effect.GameplayEffect);
	}
}

FVector AGRPlayerState::GetGroundPointUsingLineTrace(AActor* SpawnedActor)
{
	if (!SpawnedActor)
	{
		return FVector::ZeroVector;
	}
	if (!GetWorld())
	{
		return SpawnedActor->GetActorLocation();
	}

	static const FVector FallDirection = FVector(0, 0, -1.0f);
	static const float CheckDistance = 1000.0f;
	FVector Start = SpawnedActor->GetActorLocation();
	FVector Result = Start;
	FVector End = Start + FallDirection * (CheckDistance);
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(SpawnedActor);
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
	{
		Result.Z = HitResult.ImpactPoint.Z;
	}

	return Result;
}

void AGRPlayerState::PlaceActorOnGround(AActor* SpawnedActor)
{
	if (!SpawnedActor)
	{
		return;
	}

	FVector BoxOrigin;
	FVector BoxExtent;
	SpawnedActor->GetActorBounds(true, BoxOrigin, BoxExtent);
	FVector NewLocation = GetGroundPointUsingLineTrace(SpawnedActor);
	NewLocation.Z += BoxExtent.Z;
	SpawnedActor->SetActorLocation(NewLocation);
}
