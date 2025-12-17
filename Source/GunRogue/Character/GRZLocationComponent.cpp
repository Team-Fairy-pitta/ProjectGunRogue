#include "Character/GRZLocationComponent.h"
#include "Character/GRCharacter.h"
#include "AI/Character/GRAICharacter.h"
#include "AbilitySystem/GRAbilitySystemComponent.h"
#include "AbilitySystem/Attributes/GRHealthAttributeSet.h"

UGRZLocationComponent::UGRZLocationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.TickInterval = 1.0f;
}

void UGRZLocationComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UGRZLocationComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (CheckOutOfWorld())
	{
		SetHealthZero();
	}
}

bool UGRZLocationComponent::CheckOutOfWorld()
{
	AActor* ComponentOwner = GetOwner();
	if (!IsValid(ComponentOwner))
	{
		return false;
	}

	const FVector& Location = ComponentOwner->GetActorLocation();
	if (Location.Z < WorldBorderMin || WorldBorderMax < Location.Z)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void UGRZLocationComponent::SetHealthZero()
{
	UAbilitySystemComponent* ASC = nullptr;

	AActor* ComponentOwner = GetOwner();
	if (!IsValid(ComponentOwner))
	{
		return;
	}

	if (ComponentOwner->IsA(AGRCharacter::StaticClass()))
	{
		AGRCharacter* GRCharacter = Cast<AGRCharacter>(ComponentOwner);
		if (IsValid(GRCharacter))
		{
			ASC = GRCharacter->GetAbilitySystemComponent();
		}
	}
	else if (ComponentOwner->IsA(AGRAICharacter::StaticClass()))
	{
		AGRAICharacter* AICharacter = Cast<AGRAICharacter>(ComponentOwner);
		if (IsValid(AICharacter))
		{
			ASC = AICharacter->GetAbilitySystemComponent();
		}
	}

	if (!ASC)
	{
		UE_LOG(LogTemp, Error, TEXT("Owner Of UGRZLocationComponent does not have ASC!"));
		return;
	}

	ASC->SetNumericAttributeBase(UGRHealthAttributeSet::GetHealthAttribute(), 0.0f);
}

