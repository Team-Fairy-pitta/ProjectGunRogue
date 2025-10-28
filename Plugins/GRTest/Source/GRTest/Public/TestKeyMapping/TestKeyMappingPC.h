#pragma once

#include "Player/GRPlayerController.h"
#include "TestKeyMappingPC.generated.h"

class UUserWidget;

UCLASS()
class GRTEST_API ATestKeyMappingPC : public AGRPlayerController
{
	GENERATED_BODY()
	
public:
	ATestKeyMappingPC();
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Widget")
	TSubclassOf<UUserWidget> TestWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TObjectPtr<UUserWidget> TestWidgetClassInstance;
};
