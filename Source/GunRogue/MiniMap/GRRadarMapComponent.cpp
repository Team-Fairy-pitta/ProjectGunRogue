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

void UGRRadarMapComponent::InitializeWidget()
{
	if (RadarMapWidget)
	{
		return;
	}

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (!OwnerPawn || !OwnerPawn->IsLocallyControlled())
	{
		return;
	}

	if (!RadarMapWidgetClass)
	{
		return;
	}

	APlayerController* PC = Cast<APlayerController>(OwnerPawn->GetController());
	if (!PC)
	{
		return;
	}

	RadarMapWidget = CreateWidget<UGRRadarMapWidget>(PC, RadarMapWidgetClass);
	if (RadarMapWidget)
	{
		RadarMapWidget->AddToViewport();
		UE_LOG(LogTemp, Warning, TEXT("Radar Widget Initialized for %s"), *OwnerPawn->GetName());
	}
}

void UGRRadarMapComponent::BeginPlay()
{
	Super::BeginPlay();

	TagMinimapShow = FGameplayTag::RequestGameplayTag("Minimap.Show");

	InitializeWidget();

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn && OwnerPawn->IsLocallyControlled())
	{
		GetWorld()->GetTimerManager().SetTimer(
			ScanTimer,
			this,
			&UGRRadarMapComponent::ScanRadar,
			0.1f,
			true
		);
	}
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

	DrawDebugSphere(
		GetWorld(),
		GetOwner()->GetActorLocation(),          
		ScanRadius,      
		16,              
		FColor::Green,   
		false,           
		0.1f             
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
			continue;
		}

		// Minimap.Type.* 태그 추출
		FGameplayTag BaseTag = FGameplayTag::RequestGameplayTag("Minimap.Type");
		FGameplayTag RadarTypeTag;

		for (const FGameplayTag& T : Tags)
		{
			if (T.MatchesTag(BaseTag))
			{
				RadarTypeTag = T;
				break;
			}
		}

		if (!RadarTypeTag.IsValid())
		{
			continue;
		}

		FRadarTargetInfo Info;
		Info.TargetActor = HitActor;
		Info.RadarPosition = ConvertWorldToRadarPosition(HitActor->GetActorLocation());
		Info.RadarTag = RadarTypeTag;

		TargetList.Add(Info);
	}

	RadarMapWidget->UpdateRadar(TargetList);

}

FVector2D UGRRadarMapComponent::ConvertWorldToRadarPosition(const FVector& WorldLocation) const
{
	// 카메라 기준
	/*FVector OwnerLoc = GetOwner()->GetActorLocation();

	FRotator CamRot = GetWorld()->GetFirstPlayerController()->PlayerCameraManager->GetCameraRotation();

	FVector Dir = WorldLocation - OwnerLoc;
	FVector LocalDir = CamRot.UnrotateVector(Dir);

	float X = FMath::Clamp(LocalDir.Y / ScanRadius, -1.f, 1.f);
	float Y = FMath::Clamp(LocalDir.X / ScanRadius, -1.f, 1.f);

	return FVector2D(X, Y);*/

	// 플레이어 캐릭터 기준
	FVector OwnerLoc = GetOwner()->GetActorLocation();

	FRotator OwnerRot = GetOwner()->GetActorRotation();

	FVector Dir = WorldLocation - OwnerLoc;

	FVector LocalDir = OwnerRot.UnrotateVector(Dir);

	float X = FMath::Clamp(LocalDir.Y / ScanRadius, -1.f, 1.f); 
	float Y = FMath::Clamp(LocalDir.X / ScanRadius, -1.f, 1.f); 

	return FVector2D(X, Y);
}
