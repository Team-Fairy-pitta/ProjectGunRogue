#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GRAnimNotify_SpawnProjectile.generated.h"


class AGRBladeWaveProjectile;
class UStaticMeshComponent;

UCLASS()
class GUNROGUE_API UGRAnimNotify_SpawnProjectile : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	UGRAnimNotify_SpawnProjectile();

	virtual FString GetNotifyName_Implementation() const override;

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BladeWave|Spawn")
	TSubclassOf<AGRBladeWaveProjectile> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BladeWave|Spawn")
	FName SocketName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BladeWave|Spawn")
	bool bUseAimRotation;
};
