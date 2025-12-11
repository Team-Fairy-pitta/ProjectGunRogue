// GRGoodsActor.cpp


#include "Goods/GRGoodsActor.h"

#include "GRGoodsDefinition.h"
#include "Character/GRCharacter.h"
#include "Components/SphereComponent.h"
#include "Player/GRPlayerState.h"

AGRGoodsActor::AGRGoodsActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	SetRootComponent(SceneRoot);

	StaticMeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMeshComponent"));
	StaticMeshComponent->SetupAttachment(SceneRoot);

	SphereComponent = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComponent"));
	SphereComponent->InitSphereRadius(80.f);
	SphereComponent->SetupAttachment(SceneRoot);

	SphereComponent->OnComponentBeginOverlap.AddDynamic(this, &AGRGoodsActor::OnBeginOverlap);
}

void AGRGoodsActor::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		AActor* GoodsOwner = GetOwner();
		if (GoodsOwner != nullptr)
		{
			AGRPlayerState* GoodsOwnerPlayerState = Cast<AGRPlayerState>(GoodsOwner);
			if (IsValid(GoodsOwnerPlayerState))
			{
				AController* OwnerController = GoodsOwnerPlayerState->GetOwningController();
				if (!OwnerController->IsLocalController())
				{
					SetInvisible();
					return;
				}
			}
		}
	}

	if (GoodsDefinition)
	{
		InitGoods(GoodsDefinition);
	}
}

bool AGRGoodsActor::IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget,
	const FVector& SrcLocation) const
{
	bool DefaultNetRelevant = Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);

	AActor* GoodsOwner = GetOwner();
	if (GoodsOwner)
	{
		if (RealViewer && RealViewer->IsA<APlayerController>())
		{
			const APlayerController* ViewerController = Cast<APlayerController>(RealViewer);
			if (IsValid(ViewerController))
			{
				bool bIsOwnClient = ViewerController->GetPlayerState<AGRPlayerState>() == GoodsOwner;
				return bIsOwnClient && DefaultNetRelevant;
			}
		}
	}
	
	return DefaultNetRelevant;
}

void AGRGoodsActor::MulticastRPC_InitGoods_Implementation(UGRGoodsDefinition* InGoodsDefinition)
{
	InitGoods(InGoodsDefinition);

	PlaceActorOnGround();
}

void AGRGoodsActor::InitGoods(UGRGoodsDefinition* InGoodsDefinition)
{
	GoodsDefinition = InGoodsDefinition;

	if (!GoodsDefinition)
	{
		return;
	}

	if (!StaticMeshComponent)
	{
		return;
	}

	StaticMeshComponent->SetStaticMesh(GoodsDefinition->GoodsMesh);

	Amount = GoodsDefinition->GoodsAmount;
}

void AGRGoodsActor::SetInvisible()
{
	if (!StaticMeshComponent)
	{
		return;
	}

	StaticMeshComponent->SetVisibility(false, true);
}

void AGRGoodsActor::PlaceActorOnGround()
{
	FVector NewLocation = GetGroundPointUsingLineTrace();
	this->SetActorLocation(NewLocation);
}

FVector AGRGoodsActor::GetGroundPointUsingLineTrace()
{
	if (!GetWorld())
	{
		return this->GetActorLocation();
	}

	static const FVector FallDirection = FVector(0, 0, -1.0f);
	static const float CheckDistance = 1000.0f;
	FVector Start = this->GetActorLocation();
	FVector Result = Start;
	FVector End = Start + FallDirection * (CheckDistance);
	FHitResult HitResult;
	FCollisionQueryParams Params;
	Params.AddIgnoredActor(this);
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_Visibility, Params))
	{
		Result.Z = HitResult.ImpactPoint.Z;
	}

	return Result;
}

void AGRGoodsActor::OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 BodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!HasAuthority())
	{
		return;
	}

	AGRCharacter* Character = Cast<AGRCharacter>(OtherActor);
	if (!Character)
	{
		return;
	}

	AGRPlayerState* PS = Character->GetPlayerState<AGRPlayerState>();
	if (!PS)
	{
		return;
	}

	if (!GoodsDefinition)
	{
		return;
	}

	if (GoodsDefinition->GoodsType == FName("Gold"))
	{
		PS->AddGold(Amount);
	}
	else if (GoodsDefinition->GoodsType == FName("Gem"))
	{
		PS->AddMetaGoods(Amount);
	}

	Destroy();
}


