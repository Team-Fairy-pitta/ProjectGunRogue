#pragma once

#include "GameFramework/Character.h"
#include "AbilitySystemInterface.h"
#include "GRCharacter.generated.h"

class USpringArmComponent;
class UCameraComponent;
class UBoxComponent;
class AGRPlayerController;
class AGRPlayerState;
class UGRAbilitySystemComponent;
class UGRInputHandleComponent;
class UGRInteractionComponent;
class UGRAttachmentComponent;
class UGRZLocationComponent;
class UGRPawnData;
class UNiagaraSystem;
class UGRDroneManagerComponent;

UCLASS()
class GUNROGUE_API AGRCharacter : public ACharacter, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	AGRCharacter();
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION(BlueprintCallable, Category = "AITCharacter")
	AGRPlayerController* GetGRPlayerController() const;

	UFUNCTION(BlueprintCallable, Category = "AITCharacter")
	AGRPlayerState* GetGRPlayerState() const;

	UFUNCTION(BlueprintCallable, Category = "AITCharacter")
	virtual UGRAbilitySystemComponent* GetGRAbilitySystemComponent() const;

	const UGRPawnData* GetPawnData() const { return PawnData; }
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPC_OnDead();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Pawn Data")
	TObjectPtr<UGRPawnData> PawnData;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UGRInputHandleComponent> InputHandleComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UGRInteractionComponent> InteractionComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UGRAttachmentComponent> AttachmentComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UGRZLocationComponent> ZLocationComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<USpringArmComponent> SpringArmComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Components")
	TObjectPtr<UCameraComponent> CameraComponent;
	
	// 장착된 무기 메시 헬퍼
	UFUNCTION(BlueprintCallable, Category = "GRCharacter|Weapon")
	USkeletalMeshComponent* GetEquippedWeaponMesh() const;

	UFUNCTION(BlueprintCallable, Category = "GRCharacter|Weapon")
	UStaticMeshComponent* GetEquippedWeaponStaticMesh() const;

protected:
	void OnDead_ProcessAuth();
	void OnDead_ProcessLocal();
	void OnDead_ProcessRagdoll();

#pragma region WeaponFX
public:
	// 무기 이펙트/사운드 관련 RPC
	UFUNCTION(Server, UnReliable)
	void ServerRPC_PlayFireFX(const FVector& MuzzleLocation, const FVector& TracerEndPoint);

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayFireFX(const FVector& MuzzleLocation, const FVector& TracerEndPoint);

	UFUNCTION(Server, UnReliable)
	void ServerRPC_PlayImpactFX(const FVector& ImpactLocation);

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayImpactFX(const FVector& ImpactLocation);

	UFUNCTION(Server, UnReliable)
	void ServerRPC_PlayEmptyFireFX(const FVector& MuzzleLocation);

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayEmptyFireFX(const FVector& MuzzleLocation);

	UFUNCTION(Server, Reliable)
	void ServerRPC_PlayReloadSound(float ReloadRate = 1.0f);

	UFUNCTION(NetMulticast, Unreliable)
	void Multicast_PlayReloadSound(float ReloadRate = 1.0f);

	// 로컬 FX 재생 함수 (클라이언트 예측용)
	void PlayFireFXLocal(const FVector& MuzzleLocation, const FVector& TraceEnd);
	void PlayEmptyFireFXLocal(const FVector& MuzzleLocation);
	void PlayImpactFXLocal(const FVector& ImpactLocation);
#pragma endregion WeaponFX

#pragma region SkillFX
	UFUNCTION(NetMulticast, Reliable)
	void Multicast_PlaySkillSpawnEffects(
		const TArray<FVector>& Locations,
		UNiagaraSystem* NiagaraEffect,
		UParticleSystem* CascadeEffect,
		float EffectScale,
		USoundBase* SpawnSound,
		float EffectDuration = 0.0f);
#pragma endregion SkillFX

#pragma region SmoothCameraControl
public:
	void SetLastControllerRotation();
	void SetCharacterVisibilityForCamera(bool InVisibility);

	UFUNCTION(BlueprintCallable)
	void AddControllerYawSmooth_Temporal(float Value);

	UFUNCTION(BlueprintCallable)
	void AddControllerPitchSmooth_Temporal(float Value);

	UFUNCTION(BlueprintCallable)
	void AddControllerYawSmooth(float Value);

	UFUNCTION(BlueprintCallable)
	void AddControllerPitchSmooth(float Value);

	UFUNCTION(BlueprintCallable)
	void ReturnToLastControllerRotation();

	UFUNCTION(BlueprintCallable)
	void SetFirstPersonViewSmooth();

	UFUNCTION(BlueprintCallable)
	void SetThirdPersonViewSmooth();

protected:
	void ApplySmoothCameraControl_Rotation(float DeltaTime);
	void ApplySmoothCameraControl_CameraArm(float DeltaTime);
	void AttachCameraArmToCapsule();
	void AttachCameraArmToHead();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunRogue|CameraControl")
	FVector ThirdPerson_CameraArmLocation = FVector(0.0f, 25.0f, 75.0f);

	// [NOTE] [TODO] 적절한 카메라 위치를 찾아야 함
	// [NOTE] 이유는 모르겠는데 X가 좌우, Y가 위아래, Z가 앞뒤로 적용되며, 부호도 이상함.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunRogue|CameraControl")
	FVector FirstPerson_CameraArmLocation = FVector(0.0f, -6.0f, 4.0f);

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunRogue|CameraControl")
	float ThirdPerson_CameraArmLength = 150.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunRogue|CameraControl")
	float FirstPerson_CameraArmLength = 0.0f;

	FVector TargetCameraArmLocation;
	float TargetCameraArmLength;

	// 플레이어가 지정한 최근(Last) 컨트롤러 방향을 저장
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FQuat LastControllerRotation;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunRogue|CameraControl")
	float SmoothAlpha = 0.9f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GunRogue|CameraControl")
	float SmoothSpeed = 15.0f;

	FQuat TargetCameraRotation;

	bool bIsCameraAttachedToHead;

#pragma endregion
};
