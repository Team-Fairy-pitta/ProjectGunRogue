// GRGoodsActor.h

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GRGoodsActor.generated.h"

class UGRGoodsDefinition;
class USphereComponent;

UCLASS()
class GUNROGUE_API AGRGoodsActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AGRGoodsActor();
	virtual void BeginPlay() override;
	virtual bool IsNetRelevantFor(const AActor* RealViewer, const AActor* ViewTarget, const FVector& SrcLocation) const override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_InitGoods();

	void InitGoods();

	void SetInvisible();
	
	int32 GetGoodsAmount() const { return Amount; }

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GunRogue")
	TObjectPtr<UGRGoodsDefinition> GoodsDefinition;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USphereComponent> SphereComponent;

private:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
					UPrimitiveComponent* OtherComp, int32 BodyIndex,
					bool bFromSweep, const FHitResult& SweepResult);

	int32 Amount = 0;
};
