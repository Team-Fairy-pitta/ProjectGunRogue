#include "MiniMap/GRRadarMapComponent.h"
#include "UI/MiniMap/GRRadarMapWidget.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/KismetSystemLibrary.h"

UGRRadarMapComponent::UGRRadarMapComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGRRadarMapComponent::BeginPlay()
{
	Super::BeginPlay();

	TagMinimapShow = FGameplayTag::RequestGameplayTag("MiniMap.Show");

	if (RadarMapWidgetClass)
	{
		RadarMapWidget = CreateWidget<UGRRadarMapWidget>(GetWorld(), RadarMapWidgetClass);
		if (RadarMapWidget)
		{
			RadarMapWidget->AddToViewport();
		}
	}

	GetWorld()->GetTimerManager().SetTimer(
		ScanTimer, 
		this, 
		&UGRRadarMapComponent::ScanRadar, 
		UpdateInterval, 
		true
	);
}

void UGRRadarMapComponent::ScanRadar()
{
	UE_LOG(LogTemp, Warning, TEXT("Scan"));

	if (!RadarMapWidget || !GetOwner())
	{
		UE_LOG(LogTemp, Warning, TEXT("Map return"));
		return;		
	}

	TArray<AActor*> FoundActors;

	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		GetOwner()->GetActorLocation(),
		ScanRadius,
		{ UEngineTypes::ConvertToObjectType(ECC_WorldDynamic) },
		AActor::StaticClass(),
		{ GetOwner() },
		FoundActors
	);

	TArray<FRadarTargetInfo> TargetList;

	for (AActor* HitActor : FoundActors)
	{
		UE_LOG(LogTemp, Warning, TEXT("HitActor"));
		if (!HitActor || HitActor == GetOwner())
		{
			UE_LOG(LogTemp, Warning, TEXT("HitActor continue"));
			continue;
		}

		UAbilitySystemComponent* ASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);

		if (!ASC)
		{
			UE_LOG(LogTemp, Warning, TEXT("ASC continue"));
			continue;
		}

		FGameplayTagContainer Tags;
		ASC->GetOwnedGameplayTags(Tags);

		if (!Tags.HasTag(TagMinimapShow))
		{
			UE_LOG(LogTemp, Warning, TEXT("Minimap.Show continue"));
			continue;
		}

		FGameplayTag RadarTypeTag;

		if (Tags.HasTag(FGameplayTag::RequestGameplayTag("Minimap.Type.Player")))
		{
			RadarTypeTag = FGameplayTag::RequestGameplayTag("Minimap.Type.Player");
		}
		else if (Tags.HasTag(FGameplayTag::RequestGameplayTag("Minimap.Type.Enemy")))
		{
			RadarTypeTag = FGameplayTag::RequestGameplayTag("Minimap.Type.Enemy");
		}
		else if (Tags.HasTag(FGameplayTag::RequestGameplayTag("Minimap.Type.Item")))
		{
			RadarTypeTag = FGameplayTag::RequestGameplayTag("Minimap.Type.Item");
			
		}
		UE_LOG(LogTemp, Warning, TEXT("Tag"));

		FRadarTargetInfo Info;
		Info.TargetActor = HitActor;
		Info.RadarPosition = ConvertWorldToRadarPosition(HitActor->GetActorLocation());
		Info.RadarTag = RadarTypeTag;

		TargetList.Add(Info);
	}

	UE_LOG(LogTemp, Warning, TEXT("Update"));
	RadarMapWidget->UpdateRadar(TargetList);

}

FVector2D UGRRadarMapComponent::ConvertWorldToRadarPosition(const FVector& WorldLocation) const
{
	FVector OwnerLoc = GetOwner()->GetActorLocation();
	FRotator OwnerRot = GetOwner()->GetActorRotation();

	FVector Dir = WorldLocation - OwnerLoc;

	FVector LocalDir = OwnerRot.UnrotateVector(Dir);

	float X = FMath::Clamp(LocalDir.X / ScanRadius, -1.f, 1.f);
	float Y = FMath::Clamp(LocalDir.Y / ScanRadius, -1.f, 1.f);

	return FVector2D(X, Y);
}
