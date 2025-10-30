#include "TestInputConfig/TestAddAndRemoveAbility.h"
#include "Character/GRCharacter.h"
#include "Components/SceneComponent.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"

ATestAddAndRemoveAbility::ATestAddAndRemoveAbility()
{
	PrimaryActorTick.bCanEverTick = false;

	SceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("SceneRoot"));
	SetRootComponent(SceneRoot);

	Add = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Add"));
	Remove = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Remove"));

	Add->SetupAttachment(SceneRoot);
	Remove->SetupAttachment(SceneRoot);

	AddBox = CreateDefaultSubobject<UBoxComponent>(TEXT("AddBox"));
	RemoveBox = CreateDefaultSubobject<UBoxComponent>(TEXT("RemoveBox"));

	AddBox->SetupAttachment(Add);
	RemoveBox->SetupAttachment(Remove);

	bIsAdded = false;
}

void ATestAddAndRemoveAbility::BeginPlay()
{
	Super::BeginPlay();

	if (AddBox)
	{
		AddBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnAdd);
	}

	if (RemoveBox)
	{
		RemoveBox->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnRemove);
	}
}

void ATestAddAndRemoveAbility::OnAdd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bIsAdded)
	{
		return;
	}
	bIsAdded = true;

	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Cyan, FString(TEXT("OnAdd")));

	AGRCharacter* GRCharacter = Cast<AGRCharacter>(OtherActor);
	if (!IsValid(GRCharacter))
	{
		return;
	}

	UGRAbilitySystemComponent* ASC = GRCharacter->GetGRAbilitySystemComponent();
	if (!IsValid(ASC))
	{
		return;
	}

	if (!AbilitySet)
	{
		return;
	}

	AbilitySet->GiveToAbilitySystem(ASC, &Handles);
}

void ATestAddAndRemoveAbility::OnRemove(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bIsAdded)
	{
		return;
	}
	bIsAdded = false;

	GEngine->AddOnScreenDebugMessage(-1, 10, FColor::Cyan, FString(TEXT("OnRemove")));

	AGRCharacter* GRCharacter = Cast<AGRCharacter>(OtherActor);
	if (!IsValid(GRCharacter))
	{
		return;
	}

	UGRAbilitySystemComponent* ASC = GRCharacter->GetGRAbilitySystemComponent();
	if (!IsValid(ASC))
	{
		return;
	}

	Handles.TakeFromAbilitySystem(ASC);
}

