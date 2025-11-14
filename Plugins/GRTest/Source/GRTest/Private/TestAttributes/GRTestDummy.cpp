#include "TestAttributes/GRTestDummy.h"

AGRTestDummy::AGRTestDummy()
{
 	
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	AbilitySystemComponent->SetIsReplicated(true);

	HealthAttributeSet = CreateDefaultSubobject<UGRHealthAttributeSet>(TEXT("HealthSet"));
}


void AGRTestDummy::BeginPlay()
{
	Super::BeginPlay();
	
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}

	if (HealthAttributeSet)
	{
		HealthAttributeSet->OnHealthChanged.AddUObject(this, &AGRTestDummy::PrintHealthChanged);
		HealthAttributeSet->OnShieldChanged.AddUObject(this, &AGRTestDummy::PrintShieldChanged);
		HealthAttributeSet->OnShieldBroken.AddUObject(this, &AGRTestDummy::PrintShieldBroken);
		HealthAttributeSet->OnOutOfHealth.AddUObject(this, &AGRTestDummy::OnDummyDeath);
	}

}

void AGRTestDummy::PrintHealthChanged(AActor* EventInstigator, AActor* Causer, const FGameplayEffectSpec* Spec, float Magnitude, float OldValue, float NewValue)
{
	UE_LOG(LogTemp, Warning, TEXT("Dummy Health Changed: %.1f -> %.1f (Delta: %.1f)"), OldValue, NewValue, NewValue - OldValue);

	if (!GEngine)
	{
		UE_LOG(LogTemp, Error, TEXT("GEngine is NULL! Cannot print to screen"));
		return;
	}

	if (GEngine)
	{
		FString Message = FString::Printf(TEXT("HP: %.1f -> %.1f (%.1f)"),
			OldValue, NewValue, NewValue - OldValue);

		// 피해면 빨강, 회복이면 초록
		FColor Color = (NewValue < OldValue) ? FColor::Red : FColor::Green;

		GEngine->AddOnScreenDebugMessage(-1, 3.0f, Color, Message);
	}
}

void AGRTestDummy::PrintShieldChanged(AActor* EventInstigator, AActor* Causer, const FGameplayEffectSpec* Spec, float Magnitude, float OldValue, float NewValue)
{
	UE_LOG(LogTemp, Warning, TEXT("Dummy Shield Changed: %.1f -> %.1f (Delta: %.1f)"), OldValue, NewValue, NewValue - OldValue);

	if (GEngine)
	{
		FString Message = FString::Printf(TEXT("Shield: %.1f -> %.1f (%.1f)"),
			OldValue, NewValue, NewValue - OldValue);
		// 피해면 파랑, 회복이면 하늘색
		FColor Color = (NewValue < OldValue) ? FColor::Blue : FColor::Cyan;
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, Color, Message);
	}
}

void AGRTestDummy::PrintShieldBroken(AActor* EventInstigator, AActor* Causer, const FGameplayEffectSpec* Spec, float Magnitude, float OldValue, float NewValue)
{
	UE_LOG(LogTemp, Warning, TEXT("Dummy Shield Broken!"));
	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Orange, TEXT("Shield Broken!"));
	}
}

void AGRTestDummy::OnDummyDeath(AActor* EventInstigator, AActor* Causer, const FGameplayEffectSpec* Spec,
	float Magnitude, float OldValue, float NewValue)
{
	UE_LOG(LogTemp, Warning, TEXT("Dummy Died - Destroying Actor"));

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("Dummy Died!"));
	}

	Destroy();
}
