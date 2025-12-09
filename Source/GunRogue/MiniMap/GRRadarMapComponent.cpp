#include "MiniMap/GRRadarMapComponent.h"
#include "UI/MiniMap/GRRadarMapWidget.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MiniMap/GRRadarTagComponent.h"
#include "GameFramework/Character.h"
#include "UI/BattleHUD/GRBattleHUDWidget.h"
#include "Player/Battle/GRBattlePlayerController.h"


UGRRadarMapComponent::UGRRadarMapComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGRRadarMapComponent::InitRadarWidget()
{
	if (RadarMapWidgetInstance)
	{
		return;
	}

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn || !OwnerPawn->IsLocallyControlled())
	{
		return;
	}

	AGRBattlePlayerController* GRPC = Cast<AGRBattlePlayerController>(OwnerPawn->GetController());
	if (!GRPC)
	{
		return;
	}

	UGRBattleHUDWidget* HUDWidget = Cast<UGRBattleHUDWidget>(GRPC->GetBattleHUDWidget());

	if(!HUDWidget)
	{
		return;
	}

	RadarMapWidgetInstance = HUDWidget->GetMinimapWidget();

	GetWorld()->GetTimerManager().SetTimer(
		ScanTimer,
		this,
		&UGRRadarMapComponent::ScanRadar,
		0.1f,
		true
	);
}

void UGRRadarMapComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn && OwnerPawn->IsLocallyControlled())
	{
		GetWorld()->GetTimerManager().ClearTimer(ScanTimer);
	}
}

void UGRRadarMapComponent::ScanRadar()
{
	if (!RadarMapWidgetInstance)
	{
		UE_LOG(LogTemp, Error, TEXT("RadarMapWidgetInstance is INVALID"));
		return;
	}

	if (!GetOwner())
	{
		UE_LOG(LogTemp, Error, TEXT("Onwer of UGRRadarMapComponent is INVALID"));
		return;
	}

	TArray<AActor*> FoundActors;

	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		GetOwner()->GetActorLocation(),
		ScanRadius,
		{ 
			UEngineTypes::ConvertToObjectType(ECC_Pawn),
			UEngineTypes::ConvertToObjectType(ECC_WorldDynamic),
			UEngineTypes::ConvertToObjectType(ECC_PhysicsBody)
		},
		AActor::StaticClass(),
		{ GetOwner() },
		FoundActors
	);

#if WITH_EDITOR
	DrawDebugSphere(
		GetWorld(),
		GetOwner()->GetActorLocation(),
		ScanRadius,
		16,
		FColor::Green,
		false,
		0.1f
	);
#endif

	TArray<FRadarTargetInfo> TargetList;

	for (AActor* HitActor : FoundActors)
	{
		if (!HitActor || HitActor == GetOwner())
		{
			continue;
		}

		UGRRadarTagComponent* RadarTagComp = HitActor->FindComponentByClass<UGRRadarTagComponent>();

		if (!RadarTagComp)
		{
			continue;
		}

		FGameplayTag RadarTypeTag = RadarTagComp->RadarTag;

		if (!RadarTypeTag.IsValid())
		{
			continue;
		}

		FRadarTargetInfo Info;
		Info.TargetActor = HitActor;
		Info.RadarTag = RadarTypeTag;
		Info.NormalizedTargetDirection = GetNormalizedTargetDirection(HitActor->GetActorLocation());

		TargetList.Add(Info);
	}

	RadarMapWidgetInstance->UpdateRadar(TargetList);

}

FVector2D UGRRadarMapComponent::GetNormalizedTargetDirection(FVector TargetLocation) const
{
	FVector OwnerLocation = GetOwner()->GetActorLocation();
	OwnerLocation.Z = 0;
	TargetLocation.Z = 0;

	AActor* Owner = GetOwner();
	FRotator OwnerRotator;

	ACharacter* Character = Cast<ACharacter>(Owner);
	if (IsValid(Character))
	{
		OwnerRotator = Character->GetControlRotation();
	}
	else
	{
		OwnerRotator = GetOwner()->GetActorRotation();
	}
	OwnerRotator.Pitch = 0;

	FVector Direction = TargetLocation - OwnerLocation;
	Direction = OwnerRotator.UnrotateVector(Direction);

	FVector2D Direction2D(Direction.X, Direction.Y);
	return Direction2D / (2 * ScanRadius);
}
