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

	TArray<FTracedActor> TracedActors;
	for (const FHitResult& HitResult : HitResults)
	{
		FTracedActor& Item = TracedActors.AddDefaulted_GetRef();
		Item.Actor = HitResult.GetActor();
		Item.Distance = HitResult.Distance;
	}
	TracedActors.Sort(); // Item.Distance 오름차순 정렬

	for (const FTracedActor& Element : TracedActors)
	{
		AActor* HitActor = Element.Actor;
		IGRInteractableActor* InteractableActor = Cast<IGRInteractableActor>(HitActor);

		if (InteractableActor)
		{
			bool bCanInteractWith = InteractableActor->CanInteract(GetOwner());
			if (bCanInteractWith)
			{
				return HitActor;
			}
			else
			{
				// 상호 작용 가능한 Actor가 여러 개 겹쳐 있는 경우를 위한 로직
				continue;
			}
		}
		else
		{
			// 가장 가까운 Actor가 IGRInteractableActor가 아닌 경우.
			return nullptr;
		}
	}
	return nullptr;
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
