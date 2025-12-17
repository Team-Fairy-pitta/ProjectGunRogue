#include "TestAttributes/GRTestDummy.h"
#include "AbilitySystem/Attributes/GRCombatAttributeSet.h"

AGRTestDummy::AGRTestDummy()
{
 	
	PrimaryActorTick.bCanEverTick = false;

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	bReplicates = true;

	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("ASC"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	HealthAttributeSet = CreateDefaultSubobject<UGRHealthAttributeSet>(TEXT("HealthSet"));


	SphereCollision = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	SphereCollision->SetupAttachment(RootComponent);
	SphereCollision->SetSphereRadius(100.0f);
	SphereCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	SphereCollision->SetGenerateOverlapEvents(true);
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


	if (SphereCollision)
	{
		SphereCollision->OnComponentBeginOverlap.AddDynamic(
			this, &AGRTestDummy::OnOverlapBegin);
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


void AGRTestDummy::OnOverlapBegin(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	// 자기 자신 제외
	if (!OtherActor || OtherActor == this)
	{
		return;
	}

	// Target ASC 확인
	IAbilitySystemInterface* TargetASI = Cast<IAbilitySystemInterface>(OtherActor);
	if (!TargetASI)
	{
		return;
	}

	UAbilitySystemComponent* TargetASC = TargetASI->GetAbilitySystemComponent();
	if (!TargetASC || !AbilitySystemComponent)
	{
		return;
	}

	if (!DamageEffect)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TestDummy] DamageEffect not set"));
		return;
	}

	float TargetDamageReduction = 0.0f;

	if (const UGRCombatAttributeSet* TargetCombatSet =TargetASC->GetSet<UGRCombatAttributeSet>())
	{
		TargetDamageReduction = TargetCombatSet->GetDamageReduction();
	}

	const float FinalDamage = OverlapDamage * (1.0f - TargetDamageReduction);

	FGameplayEffectContextHandle EffectContext =AbilitySystemComponent->MakeEffectContext();
	EffectContext.AddSourceObject(this);

	// GE Spec 생성
	FGameplayEffectSpecHandle SpecHandle =
		AbilitySystemComponent->MakeOutgoingSpec(
			DamageEffect,
			1.0f,
			EffectContext
		);

	if (!SpecHandle.IsValid())
	{
		return;
	}

	SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Attribute.Data.Damage")), FinalDamage);

	AbilitySystemComponent->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);

	UE_LOG(LogTemp, Warning, TEXT("[TestDummy] Overlap Damage Applied: %.1f (Reduction: %.2f)"), FinalDamage, TargetDamageReduction);
}
