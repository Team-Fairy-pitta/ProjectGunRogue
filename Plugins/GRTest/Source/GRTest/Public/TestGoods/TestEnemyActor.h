// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestEnemyActor.generated.h"

class AGRGoodsActor;
class UGRGoodsDefinition;
class USphereComponent;

//실제 사용할 코드
USTRUCT(BlueprintType)
struct FDropGoodsInfo_TEST
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	TSubclassOf<AGRGoodsActor> GoodsClass;
	
	UPROPERTY(EditAnywhere)
	int32 Count = 1;
};

UCLASS()
class GRTEST_API ATestEnemyActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ATestEnemyActor();

	//실제 사용 코드
	void DropGoods();

protected:
	virtual void BeginPlay() override;

	//실제 사용 코드
	UPROPERTY(EditAnywhere, Category = "Goods")
	TArray<FDropGoodsInfo_TEST> DropGoodsList;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USceneComponent> SceneRoot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UStaticMeshComponent> StaticMeshComponent;
	
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> SphereComponent;

private:
	UFUNCTION()
	void OnBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
					UPrimitiveComponent* OtherComp, int32 BodyIndex,
					bool bFromSweep, const FHitResult& SweepResult);

	//실제 사용 코드
	void DropGoodsForEachPlayer(APlayerController* Player);
	TArray<FDropGoodsInfo_TEST> GetDropGoodsList();
	void SpawnToTargetPlayer(APlayerState* PlayerState, TSubclassOf<AGRGoodsActor> GoodsClass, int32 DropCount);

	FVector GetRanomOffsetAround() const;

	FVector GetGroundLocation(const FVector& InXY) const;
};
