#pragma once

#include "Player/GRPlayerController.h"
#include "TestKeyMappingPC.generated.h"

class UUserWidget;
class UInputMappingContext;
class UEnhancedInputUserSettings;
struct FPlayerKeyMapping;

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	// UI 초기화 관련 함수
	UFUNCTION(BlueprintCallable)
	void InitKeyMappings();

	UFUNCTION(BlueprintCallable)
	void ClearMappingWidget();

	// KeyMapping, 즉 User Setting 관련 함수
	// 아래 함수는 실제 코드에서 재활용할 수 있도록 구현함

	UFUNCTION(BlueprintCallable)
	void LoadOrCreateUserSettings();

	UFUNCTION(BlueprintCallable)
	void SaveInputUserSettings();

	UFUNCTION(BlueprintCallable)
	TMap<FName, FPlayerKeyMapping> GetPlayerKeyMappings();

	UFUNCTION(BlueprintCallable)
	void RegisterInputMappingContext(UInputMappingContext* MappingContext);

private:
	UPROPERTY()
	UEnhancedInputUserSettings* CachedUserSetting;
};
