#include "Character/Interaction/GRInteractionComponent.h"
#include "Character/Interaction/GRInteractableActor.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"

UGRInteractionComponent::UGRInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 0.1f;
}

void UGRInteractionComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	AActor* Owner = GetOwner();
	APawn* OwnerPawn = Cast<APawn>(Owner);
	if (!IsValid(OwnerPawn))
	{
		UE_LOG(LogTemp, Error, TEXT("Owner of Component is NOT Pawn"));
		return;
	}

	if (OwnerPawn->IsLocallyControlled())
	{
		FindInteractableActor();
	}
}

void UGRInteractionComponent::FindInteractableActor()
{
	AActor* HitActor = TraceForInteractable();
	if (!IsValid(HitActor))
	{
		if (FocusedActor)
		{
			OnOut(FocusedActor);
			FocusedActor = nullptr;
		}
		return;
	}

	IGRInteractableActor* InteractableActor = Cast<IGRInteractableActor>(HitActor);
	if (InteractableActor && InteractableActor->CanInteract(GetOwner()))
	{
		if (HitActor != FocusedActor)
		{
			if (FocusedActor)
			{
				OnOut(FocusedActor);
				FocusedActor = nullptr;
			}
			if (HitActor)
			{
				OnOver(HitActor);
				FocusedActor = HitActor;
			}
		}
	}
	else
	{
		if (FocusedActor)
		{
			OnOut(FocusedActor);
			FocusedActor = nullptr;
		}
	}
}

void UGRInteractionComponent::OnOver(AActor* InActor)
{
	AddOutline(InActor);

	IGRInteractableActor* InteractableActor = Cast<IGRInteractableActor>(InActor);
	if (InteractableActor)
	{
		InteractableActor->OnOver();
	}
}

void UGRInteractionComponent::OnOut(AActor* InActor)
{
	RemoveOutline(FocusedActor);

	IGRInteractableActor* InteractableActor = Cast<IGRInteractableActor>(InActor);
	if (InteractableActor)
	{
		InteractableActor->OnOut();
	}
}

AActor* UGRInteractionComponent::TraceForInteractable()
{
	AActor* Owner = GetOwner();
	APawn* OwnerPawn = Cast<APawn>(Owner);
	if (!IsValid(OwnerPawn))
	{
		UE_LOG(LogTemp, Error, TEXT("Owner of Component is NOT Pawn"));
		return nullptr;
	}

	AController* OwnerController = OwnerPawn->GetController();
	if (!IsValid(OwnerController))
	{
		UE_LOG(LogTemp, Error, TEXT("OwnerController is INVALID"));
		return nullptr;
	}

	FVector StartLocation;
	FRotator CameraRotation;
	OwnerController->GetPlayerViewPoint(StartLocation, CameraRotation);
	FVector EndLocation = StartLocation + CameraRotation.Vector() * InteractionDistance;

	AActor* HitActor = DoLineTrace(StartLocation, EndLocation);

	// [NOTE] 상호 작용 디버깅이 필요하다면, 아래 코드를 활성화
#if 0
	if (bHit)
	{
		DrawDebugLine(
			GetWorld(), StartLocation, HitResult.ImpactPoint, FColor::Green,
			false, 1.0f, 0.0f, 3.0f);
	}
	else
	{
		DrawDebugLine(
			GetWorld(), StartLocation, EndLocation, FColor::Red,
			false, 1.0f, 0.0f, 3.0f);
	}
#endif

	return HitActor;
}

AActor* UGRInteractionComponent::DoLineTrace(FVector& StartLocation, FVector& EndLocation)
{
	TArray<FHitResult> HitResults;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(GetOwner());

	// ECC_GameTraceChannel4 = Interaction Channel
	GetWorld()->LineTraceMultiByChannel(HitResults, StartLocation, EndLocation, ECC_GameTraceChannel4, Params);

	float MinDistance = -1.0f;
	AActor* ClosestActor = nullptr;
	for (const FHitResult& HitResult : HitResults)
	{
		AActor* HitActor = HitResult.GetActor();
		IGRInteractableActor* InteractableActor = Cast<IGRInteractableActor>(HitActor);
		bool bCanInteract = InteractableActor && InteractableActor->CanInteract(GetOwner());
		if (bCanInteract)
		{
			if (MinDistance < 0 || HitResult.Distance < MinDistance)
			{
				MinDistance = HitResult.Distance;
				ClosestActor = HitActor;
			}
		}
	}
	return ClosestActor;
}

void UGRInteractionComponent::AddOutline(AActor* InActor)
{
	IGRInteractableActor* InteractableActor = Cast<IGRInteractableActor>(InActor);
	if (!InteractableActor)
	{
		UE_LOG(LogTemp, Error, TEXT("InActor is NOT IGRInteractableActor"));
		return;
	}

	for (UStaticMeshComponent* Mesh : InteractableActor->GetMeshComponents())
	{
		if (Mesh)
		{
			Mesh->SetRenderCustomDepth(true);
		}
	}
}

void UGRInteractionComponent::RemoveOutline(AActor* InActor)
{
	IGRInteractableActor* InteractableActor = Cast<IGRInteractableActor>(InActor);
	if (!InteractableActor)
	{
		UE_LOG(LogTemp, Error, TEXT("InActor is NOT IGRInteractableActor"));
		return;
	}

	for (UStaticMeshComponent* Mesh : InteractableActor->GetMeshComponents())
	{
		if (Mesh)
		{
			Mesh->SetRenderCustomDepth(false);
		}
	}
}
