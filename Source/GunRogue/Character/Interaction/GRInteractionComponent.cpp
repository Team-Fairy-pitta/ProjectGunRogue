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
		TraceForInteractable();
	}
}

void UGRInteractionComponent::TraceForInteractable()
{
	AActor* Owner = GetOwner();
	APawn* OwnerPawn = Cast<APawn>(Owner);
	if (!IsValid(OwnerPawn))
	{
		UE_LOG(LogTemp, Error, TEXT("Owner of Component is NOT Pawn"));
		return;
	}

	AController* OwnerController = OwnerPawn->GetController();
	if (!IsValid(OwnerController))
	{
		UE_LOG(LogTemp, Error, TEXT("OwnerController is INVALID"));
		return;
	}

	FVector StartLocation;
	FRotator CameraRotation;
	OwnerController->GetPlayerViewPoint(StartLocation, CameraRotation);
	FVector EndLocation = StartLocation + CameraRotation.Vector() * InteractionDistance;

	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Owner);

	bool bHit = GetWorld()->LineTraceSingleByChannel(HitResult, StartLocation, EndLocation, ECC_Visibility, Params);
	AActor* HitActor = bHit ? HitResult.GetActor() : nullptr;
	IGRInteractableActor* InteractableActor = Cast<IGRInteractableActor>(HitActor);

	if (InteractableActor)
	{
		if (HitActor != FocusedActor)
		{
			if (FocusedActor)
			{
				RemoveOutline(FocusedActor);
			}
			if (HitActor)
			{
				FocusedActor = HitActor;
				AddOutline(HitActor);
			}
		}
	}

	if (!bHit)
	{
		if (FocusedActor)
		{
			RemoveOutline(FocusedActor);
			FocusedActor = nullptr;
		}
	}

#if WITH_EDITOR
	FColor LineColor = bHit ? FColor::Green : FColor::Red;
	DrawDebugLine(GetWorld(), StartLocation, EndLocation, LineColor, false, 0.1f, 0, 1.f);
#endif
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
