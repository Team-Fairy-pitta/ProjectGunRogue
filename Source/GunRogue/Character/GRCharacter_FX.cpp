#include "Character/GRCharacter.h"
#include "Weapon/GRWeaponDefinition.h"
#include "Player/GRPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundBase.h"

void AGRCharacter::ServerRPC_PlayFireFX_Implementation(const FVector& MuzzleLocation,
	const FVector& TracerEndPoint)
{
	Multicast_PlayFireFX(MuzzleLocation, TracerEndPoint);
}

void AGRCharacter::Multicast_PlayFireFX_Implementation(const FVector& MuzzleLocation,
	const FVector& TracerEndPoint)
{
	// 로컬 클라이언트는 이미 재생했으므로 스킵
	if (IsLocallyControlled())
	{
		return;
	}

	AGRPlayerState* PS = GetGRPlayerState();
	if (!PS)
	{
		return;
	}

	UGRWeaponDefinition* WeaponDef = PS->GetCurrentWeaponDefinition();
	if (!WeaponDef)
	{
		return;
	}

	// 발사 사운드
	if (WeaponDef->FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this, WeaponDef->FireSound, MuzzleLocation, 1.0f, 1.0f);
	}

	// 머즐 플래시
	if (WeaponDef->MuzzleFlashNiagara)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this, WeaponDef->MuzzleFlashNiagara,
			MuzzleLocation, FRotator::ZeroRotator, FVector(1.0f),
			true, true, ENCPoolMethod::AutoRelease);
	}
	else if (WeaponDef->MuzzleFlashCascade)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			this, WeaponDef->MuzzleFlashCascade,
			MuzzleLocation, FRotator::ZeroRotator, FVector(1.0f),
			true, EPSCPoolMethod::AutoRelease);
	}

	// 총알 궤적
	FVector Dir = (TracerEndPoint - MuzzleLocation).GetSafeNormal();

	if (WeaponDef->BulletTracerNiagara)
	{
		UNiagaraComponent* TracerComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this, WeaponDef->BulletTracerNiagara,
			MuzzleLocation, Dir.Rotation(), FVector(1.0f),
			true, true, ENCPoolMethod::AutoRelease);
		if (TracerComponent)
		{
			TracerComponent->SetVectorParameter(FName("BeamStart"), MuzzleLocation);
			TracerComponent->SetVectorParameter(FName("BeamEnd"), TracerEndPoint);
		}
	}
	else if (WeaponDef->BulletTracerCascade)
	{
		UParticleSystemComponent* TracerComponent = UGameplayStatics::SpawnEmitterAtLocation(
			this, WeaponDef->BulletTracerCascade,
			MuzzleLocation, Dir.Rotation(), FVector(1.0f),
			true, EPSCPoolMethod::AutoRelease);
		if (TracerComponent)
		{
			TracerComponent->SetVectorParameter(FName("BeamEnd"), TracerEndPoint);
		}
	}
}

void AGRCharacter::ServerRPC_PlayImpactFX_Implementation(const FVector& ImpactLocation)
{
	Multicast_PlayImpactFX(ImpactLocation);
}

void AGRCharacter::Multicast_PlayImpactFX_Implementation(const FVector& ImpactLocation)
{
	// 로컬 클라이언트는 이미 재생했으므로 스킵
	if (IsLocallyControlled())
	{
		return;
	}

	AGRPlayerState* PS = GetGRPlayerState();
	if (!PS)
	{
		return;
	}

	UGRWeaponDefinition* WeaponDef = PS->GetCurrentWeaponDefinition();
	if (!WeaponDef)
	{
		return;
	}

	// 히트 사운드
	if (WeaponDef->ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this, WeaponDef->ImpactSound, ImpactLocation, 0.8f, 1.0f);
	}

	// 히트 이펙트
	if (WeaponDef->ImpactEffectNiagara)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this, WeaponDef->ImpactEffectNiagara,
			ImpactLocation, FRotator::ZeroRotator, FVector(1.0f),
			true, true, ENCPoolMethod::AutoRelease);
	}
	else if (WeaponDef->ImpactEffectCascade)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			this, WeaponDef->ImpactEffectCascade,
			ImpactLocation, FRotator::ZeroRotator, FVector(1.0f),
			true, EPSCPoolMethod::AutoRelease);
	}
}

void AGRCharacter::ServerRPC_PlayEmptyFireFX_Implementation(const FVector& MuzzleLocation)
{
	Multicast_PlayEmptyFireFX(MuzzleLocation);
}

void AGRCharacter::Multicast_PlayEmptyFireFX_Implementation(const FVector& MuzzleLocation)
{
	// 로컬 클라이언트는 이미 재생했으므로 스킵
	if (IsLocallyControlled())
	{
		return;
	}

	AGRPlayerState* PS = GetGRPlayerState();
	if (!PS)
	{
		return;
	}

	UGRWeaponDefinition* WeaponDef = PS->GetCurrentWeaponDefinition();
	if (!WeaponDef || !WeaponDef->EmptyFireSound)
	{
		return;
	}

	UGameplayStatics::PlaySoundAtLocation(
		this, WeaponDef->EmptyFireSound, MuzzleLocation, 1.0f, 1.0f);
}

void AGRCharacter::ServerRPC_PlayReloadSound_Implementation(float ReloadRate)
{
	Multicast_PlayReloadSound(ReloadRate);
}

void AGRCharacter::Multicast_PlayReloadSound_Implementation(float ReloadRate)
{
	AGRPlayerState* PS = GetGRPlayerState();
	if (!PS)
	{
		return;
	}

	UGRWeaponDefinition* WeaponDef = PS->GetCurrentWeaponDefinition();
	if (!WeaponDef || !WeaponDef->ReloadSound)
	{
		return;
	}

	UGameplayStatics::PlaySoundAtLocation(
		this, WeaponDef->ReloadSound, GetActorLocation(), 1.0f, ReloadRate);
}

void AGRCharacter::PlayFireFXLocal(const FVector& MuzzleLocation, const FVector& TraceEnd)
{
	AGRPlayerState* PS = GetGRPlayerState();
	if (!PS)
	{
		return;
	}

	UGRWeaponDefinition* WeaponDef = PS->GetCurrentWeaponDefinition();
	if (!WeaponDef)
	{
		return;
	}

	// 발사 사운드
	if (WeaponDef->FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(), WeaponDef->FireSound, MuzzleLocation, 1.0f, 1.0f);
	}

	// 머즐 플래시
	if (WeaponDef->MuzzleFlashNiagara)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), WeaponDef->MuzzleFlashNiagara,
			MuzzleLocation, FRotator::ZeroRotator, FVector(1.0f),
			true, true, ENCPoolMethod::AutoRelease);
	}
	else if (WeaponDef->MuzzleFlashCascade)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), WeaponDef->MuzzleFlashCascade,
			MuzzleLocation, FRotator::ZeroRotator, FVector(1.0f),
			true, EPSCPoolMethod::AutoRelease);
	}

	// 총알 궤적
	FVector Dir = (TraceEnd - MuzzleLocation).GetSafeNormal();
	if (WeaponDef->BulletTracerNiagara)
	{
		UNiagaraComponent* TracerComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(), WeaponDef->BulletTracerNiagara,
			MuzzleLocation, Dir.Rotation(), FVector(1.0f),
			true, true, ENCPoolMethod::AutoRelease);
		if (TracerComponent)
		{
			TracerComponent->SetVectorParameter(FName("BeamStart"), MuzzleLocation);
			TracerComponent->SetVectorParameter(FName("BeamEnd"), TraceEnd);
		}
	}
	else if (WeaponDef->BulletTracerCascade)
	{
		UParticleSystemComponent* TracerComponent = UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(), WeaponDef->BulletTracerCascade,
			MuzzleLocation, Dir.Rotation(), FVector(1.0f),
			true, EPSCPoolMethod::AutoRelease);
		if (TracerComponent)
		{
			TracerComponent->SetVectorParameter(FName("BeamEnd"), TraceEnd);
		}
	}
}
void AGRCharacter::PlayEmptyFireFXLocal(const FVector& MuzzleLocation)
{
	AGRPlayerState* PS = GetGRPlayerState();
	if (!PS)
	{
		return;
	}

	UGRWeaponDefinition* WeaponDef = PS->GetCurrentWeaponDefinition();
	if (!WeaponDef || !WeaponDef->EmptyFireSound)
	{
		return;
	}

	UGameplayStatics::PlaySoundAtLocation(
		this, WeaponDef->EmptyFireSound, MuzzleLocation, 1.0f, 1.0f);
}
void AGRCharacter::PlayImpactFXLocal(const FVector& ImpactLocation)
{
	AGRPlayerState* PS = GetGRPlayerState();
	if (!PS)
	{
		return;
	}

	UGRWeaponDefinition* WeaponDef = PS->GetCurrentWeaponDefinition();
	if (!WeaponDef)
	{
		return;
	}

	// 히트 사운드
	if (WeaponDef->ImpactSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this, WeaponDef->ImpactSound, ImpactLocation, 0.8f, 1.0f);
	}

	// 히트 이펙트
	if (WeaponDef->ImpactEffectNiagara)
	{
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			this, WeaponDef->ImpactEffectNiagara,
			ImpactLocation, FRotator::ZeroRotator, FVector(1.0f),
			true, true, ENCPoolMethod::AutoRelease);
	}
	else if (WeaponDef->ImpactEffectCascade)
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			this, WeaponDef->ImpactEffectCascade,
			ImpactLocation, FRotator::ZeroRotator, FVector(1.0f),
			true, EPSCPoolMethod::AutoRelease);
	}
}

// === Skill FX ===

void AGRCharacter::Multicast_PlaySkillSpawnEffects_Implementation(
	const TArray<FVector>& Locations,
	UNiagaraSystem* NiagaraEffect,
	UParticleSystem* CascadeEffect,
	float EffectScale,
	USoundBase* SpawnSound)
{
	for (const FVector& Location : Locations)
	{
		// 나이아가라 우선
		if (NiagaraEffect)
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(
				GetWorld(),
				NiagaraEffect,
				Location,
				FRotator::ZeroRotator,
				FVector(EffectScale)
			);
		}
		// 캐스케이드 대체
		else if (CascadeEffect)
		{
			UGameplayStatics::SpawnEmitterAtLocation(
				GetWorld(),
				CascadeEffect,
				Location,
				FRotator::ZeroRotator,
				FVector(EffectScale)
			);
		}
	}

	// 사운드는 한 번만	재생
	if (SpawnSound && Locations.Num() > 0)
	{
		// 캐릭터 위치에서 재생
		UGameplayStatics::PlaySoundAtLocation(
			GetWorld(),
			SpawnSound,
			GetActorLocation()
		);
	}
}
