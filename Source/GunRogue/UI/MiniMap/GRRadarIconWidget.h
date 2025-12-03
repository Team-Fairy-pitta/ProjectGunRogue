#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "GRRadarIconWidget.generated.h"

class UImage;

UCLASS()
class GUNROGUE_API UGRRadarIconWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	void InitIcon(const FGameplayTag& InTag);
	void UpdateRadarPosition(const FVector2D& RadarPos);

protected:
	UPROPERTY(meta = (BindWidget))
	UImage* IconImage;

	UPROPERTY(EditAnywhere)
	float RadarScale = 0.05f;

	// 아이콘 설정용 텍스처
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* PlayerTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* EnemyTexture;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UTexture2D* ItemTexture;


private:
	FGameplayTag RadarTag;
};
