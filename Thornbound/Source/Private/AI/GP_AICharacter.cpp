// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GP_AICharacter.h"
#include "AI/GP_AIController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/GP_HealthComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/DamageEvents.h"
#include "AI/GP_AIController.h"
#include "BrainComponent.h"
#include "Components/CapsuleComponent.h"
#include "Weapon/DamageType/GP_FireDamageType.h"
#include "Weapon/DamageType/GP_ElectricityDamageType.h"
#include "Core/GP_AttackTokenSubsystem.h"

DEFINE_LOG_CATEGORY_STATIC(GP_AICharacterLog, All, All);

AGP_AICharacter::AGP_AICharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AGP_AIController::StaticClass();

	bUseControllerRotationYaw = false;
	if (GetCharacterMovement())
	{
		GetCharacterMovement()->bUseControllerDesiredRotation = true;
	}

	HealthComponent = CreateDefaultSubobject<UGP_HealthComponent>("HealthComponent");

	SetCurrentAnimState(StartAnimState);
}

void AGP_AICharacter::BeginPlay()
{
	Super::BeginPlay();
	check(HealthComponent);

	HealthComponent->OnDeath.AddDynamic(this, &AGP_AICharacter::HandleDeath);
	HealthComponent->OnDamage.AddDynamic(this, &AGP_AICharacter::HandleDamage);

	SetCurrentAnimState(StartAnimState);
}

void AGP_AICharacter::StartQTE(AActor* PlayerToDamage)
{
	if (bQTEActive) return;

	OnQTEStarted.Broadcast();
	bQTEActive = true;
	QTETarget = PlayerToDamage;
	UE_LOG(GP_AICharacterLog, Display, TEXT("%s started QTE: %s"), *GetName(), bQTEActive ? TEXT("TRUE") : TEXT("FALSE"));
	Attack(); 
	
	GetWorld()->GetTimerManager().SetTimer(AttackTimerHandle, this, &AGP_AICharacter::HandleQTEAttack, QTEAttackRateTime, true);
}

void AGP_AICharacter::FinishQTE()
{
	GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
	bQTEActive = false;
	QTETarget = nullptr;

	if (AttackAnimMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			if (!HealthComponent->IsDead())
			{
				if (!AnimInstance->Montage_IsPlaying(AttackAnimMontage))
				{
					AnimInstance->Montage_Play(AttackAnimMontage, 1.0f);
				}
				AnimInstance->Montage_SetNextSection(FName("Bite_Loop"), FName("Bite_End"), AttackAnimMontage);
				AnimInstance->Montage_JumpToSection(FName("Bite_End"), AttackAnimMontage);
			}
		}
	}

	UE_LOG(GP_AICharacterLog, Display, TEXT("%s finished QTE: %s"), *GetName(), bQTEActive ? TEXT("TRUE") : TEXT("FALSE"));
}

void AGP_AICharacter::SetCurrentAnimState(EAIAnimState NextAnimState)
{
	if (CurrentAnimState == NextAnimState) return;

	EAIAnimState PreviousAnimState = CurrentAnimState;
	CurrentAnimState = NextAnimState;

	UE_LOG(GP_AICharacterLog, Display, TEXT("EAIAnimState changed from %s to %s : %s"),
		*StaticEnum<EAIAnimState>()->GetNameStringByValue((int64)PreviousAnimState),
		*StaticEnum<EAIAnimState>()->GetNameStringByValue((int64)CurrentAnimState), *GetName()
	);
}

void AGP_AICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetWorld()->GetTimerManager().IsTimerActive(HitImpactShaderTimerHandle))
	{
		ElapsedTimeBetweenHit = GetWorld()->GetTimerManager().GetTimerElapsed(HitImpactShaderTimerHandle);
	}
}

void AGP_AICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AGP_AICharacter::Attack()
{
	UE_LOG(GP_AICharacterLog, Display, TEXT("Attack"));
	SetCurrentAnimState(EAIAnimState::Attack);

	if (AttackAnimMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			if (AnimInstance->Montage_IsPlaying(AttackAnimMontage))
			{
				FName CurrentPlayingSection = AnimInstance->Montage_GetCurrentSection(AttackAnimMontage);
				if (CurrentPlayingSection == FName("Knockback"))
				{
					GetWorld()->GetTimerManager().ClearTimer(AnimationTimerHandle);
					AnimInstance->Montage_Stop(0.0f, AttackAnimMontage);
					UE_LOG(GP_AICharacterLog, Display, TEXT("CurrentPlayingSection == Knockback"));
				}
			}
			if (!bQTEActive)
			{
				AnimInstance->Montage_Play(AttackAnimMontage);
				AnimInstance->Montage_JumpToSection(FName("Attack"), AttackAnimMontage);
			}
			else
			{
				AnimInstance->Montage_Play(AttackAnimMontage);
				AnimInstance->Montage_JumpToSection(FName("Bite_Start"), AttackAnimMontage);
				AnimInstance->Montage_SetNextSection(FName("Bite_Start"), FName("Bite_Loop"), AttackAnimMontage);
				AnimInstance->Montage_SetNextSection(FName("Bite_Loop"), FName("Bite_Loop"), AttackAnimMontage);
			}
		}
	}
}

void AGP_AICharacter::StopAttack()
{
	OnFinishedAttack.Broadcast();

	bIsDamageDone = false;
}

void AGP_AICharacter::FinishAwake()
{
	OnFinishedAwake.Broadcast();

	if (HealthComponent->IsImmortal())
	{
		HealthComponent->SetIsImmortal(!HealthComponent->IsImmortal());
	}
}

void AGP_AICharacter::HandleDeath()
{
	UE_LOG(GP_AICharacterLog, Display, TEXT("%s are dead"), *GetName());
	const auto AIController = Cast<AGP_AIController>(Controller);
	if (AIController && AIController->BrainComponent && GetMesh())
	{
		GetWorld()->GetTimerManager().ClearTimer(AttackTimerHandle);
		StopAttack();

		UGP_AttackTokenSubsystem* AttackTokenSubsystem = GetWorld()->GetSubsystem<UGP_AttackTokenSubsystem>();
		if (AttackTokenSubsystem)
		{
			AttackTokenSubsystem->ReleaseToken(AIController, 1);
		}


		AIController->BrainComponent->Cleanup();

		SetCurrentAnimState(EAIAnimState::Death);
		if (DeathAnimMontage)
		{
			UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
			if (AnimInstance)
			{
				if(AnimInstance->Montage_IsPlaying(AttackAnimMontage))
				{
					AnimInstance->Montage_Stop(0.25f, AttackAnimMontage);
				}
				float AnimLength = AnimInstance->Montage_Play(DeathAnimMontage);
				SetLifeSpan(AnimLength + 5.5f);
			}
		}

		GetCharacterMovement()->DisableMovement();
		GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
		if (OverlayMaterialInstance)
		{
			GetWorld()->GetTimerManager().ClearTimer(HitImpactShaderTimerHandle);
			FTimerDelegate InDelegate;
			InDelegate.BindUFunction(this, FName("StartImpactShaderFade"), OverlayMaterialInstance);

			GetWorld()->GetTimerManager().SetTimer(HitImpactShaderTimerHandle, InDelegate, ThresholdForCumulativeImpactShader, false);
		}
	}
}

void AGP_AICharacter::HandleDamage(float Damage, const UDamageType* DamageType, AController* InstigatedBy)
{
	if(bQTEActive) return;
	if (AttackAnimMontage && CurrentAnimState != EAIAnimState::StandUp && CurrentAnimState != EAIAnimState::Sleep && !bQTEActive)
	{
		StopAttack();
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance && AttackAnimMontage->IsValidSectionName(FName("Knockback")))
		{
			float BlendInTime = 0.45f;
			FAlphaBlendArgs AlphaBlendArgs;
			AlphaBlendArgs.BlendTime = BlendInTime;
			AlphaBlendArgs.BlendOption = EAlphaBlendOption::CubicInOut;

			FMontageBlendSettings MontageBlendSettings(AlphaBlendArgs);

			AnimInstance->Montage_PlayWithBlendSettings(AttackAnimMontage, MontageBlendSettings);
			AnimInstance->Montage_JumpToSection(FName("Knockback"), AttackAnimMontage);

			FTimerDelegate InDelegate;
			float RateTime = BlendInTime + 0.1f;
			InDelegate.BindUFunction(this, FName("HandleAnimBlendOut"), AttackAnimMontage, FName("Knockback"), RateTime);
			GetWorld()->GetTimerManager().SetTimer(AnimationTimerHandle, InDelegate, RateTime, false);
		}
	}

	int32 MagicType = -1;
	if (DamageType->IsA<UGP_ElectricityDamageType>())
	{
		MagicType = 0;
	}
	else if (DamageType->IsA<UGP_FireDamageType>())
	{
		MagicType = 1;
	}
	if (PreviousType != MagicType)
	{
		PreviousType = MagicType;
		HitCount = 0;
	}

	if(MagicType == -1) return;
	GetWorld()->GetTimerManager().ClearTimer(HitImpactShaderTimerHandle);

	if (GetMesh())
	{
		if (!OverlayMaterialInstance)
		{
			UMaterialInterface* OverlayMaterial = GetMesh()->GetOverlayMaterial();
			if (OverlayMaterial)
			{
				OverlayMaterialInstance = UMaterialInstanceDynamic::Create(OverlayMaterial, this);
				GetMesh()->SetOverlayMaterial(OverlayMaterialInstance);
			}

		}

		if (OverlayMaterialInstance)
		{
			OverlayMaterialInstance->SetScalarParameterValue(TEXT("SWITCH BETWEEN MAGIC (0=Electricity, 1=Fire)"), MagicType);
			OverlayMaterialInstance->SetScalarParameterValue(TEXT("SWITCH ON/OFF IMPACT SHADER (0=OFF, 1=ON)"), 1.0f);

			if (ElapsedTimeBetweenHit <= ThresholdForCumulativeImpactShader)
			{
				ElapsedTimeBetweenHit = 0;

				HitCount++;
				if (HitCount > 0)
				{
					float CumulativeImpactShaderAlpha = FMath::Clamp(static_cast<float>(HitCount) / MaxHitForCumulativeImpactShader, 0.f, 1.f);
					float NextCumulativeImpactIntensity = FMath::Lerp(0.0f, 1.0f, CumulativeImpactShaderAlpha);
					OverlayMaterialInstance->SetScalarParameterValue(TEXT("MORE HIT DAMAGE (0=OFF, 1=MAX)"), NextCumulativeImpactIntensity);
					//UE_LOG(GP_AICharacterLog, Display, TEXT("HandleDamage: HitCount = %d, MaxHitForCumulativeImpactShader = %d, MagicType = %d, CumulativeImpactShaderAlpha = %f "), HitCount, MaxHitForCumulativeImpactShader, MagicType, CumulativeImpactShaderAlpha);
				}
				else
				{
					//UE_LOG(GP_AICharacterLog, Display, TEXT("HandleDamage: HitCount = %d, MagicType = %d"), HitCount, MagicType);
				}

				FTimerDelegate InDelegate;
				InDelegate.BindUFunction(this, FName("StartImpactShaderFade"), OverlayMaterialInstance);

				GetWorld()->GetTimerManager().SetTimer(HitImpactShaderTimerHandle, InDelegate, ThresholdForCumulativeImpactShader, false);
			}
		}
	}
}

void AGP_AICharacter::StartImpactShaderFade(UMaterialInstanceDynamic* Material)
{
	float CurrentCumulativeImpactShader = 0;
	Material->GetScalarParameterValue(TEXT("MORE HIT DAMAGE (0=OFF, 1=MAX)"), CurrentCumulativeImpactShader);
	ElapsedTimeBetweenHit = 0;
	CurrentImpactShaderStep = 0;
	HitCount = 0;
	float TotalSteps = HitImpactShaderFadeDuration / HitImpactShaderFadeRateTime;

	FTimerDelegate InDelegate;
	InDelegate.BindUFunction(this, FName("UpdateImpactShaderFade"), Material, TotalSteps, CurrentCumulativeImpactShader);

	GetWorld()->GetTimerManager().SetTimer(HitImpactShaderTimerHandle, InDelegate, HitImpactShaderFadeRateTime, true);
}

void AGP_AICharacter::UpdateImpactShaderFade(UMaterialInstanceDynamic* Material, float StepAmount, float CurrentIntensityImpactShader)
{
	if (!Material)
	{
		GetWorld()->GetTimerManager().ClearTimer(HitImpactShaderTimerHandle);
		return;
	}

	float Alpha = FMath::Clamp(static_cast<float>(CurrentImpactShaderStep) / StepAmount, 0.f, 1.f);
	float NextImpactShaderValue = FMath::Lerp(1.0f, 0.0f, Alpha);
	//UE_LOG(GP_AICharacterLog, Display, TEXT("NextImpactShaderValue: %f, Alpha: %f, StepAmount: %f, CurrentImpactShaderStep: %d"), NextImpactShaderValue, Alpha, StepAmount, CurrentImpactShaderStep);

	float NextCumulativeImpactIntensity = FMath::Lerp(CurrentIntensityImpactShader, 0.0f, Alpha);

	Material->SetScalarParameterValue(TEXT("SWITCH ON/OFF IMPACT SHADER (0=OFF, 1=ON)"), NextImpactShaderValue);
	Material->SetScalarParameterValue(TEXT("MORE HIT DAMAGE (0=OFF, 1=MAX)"), NextCumulativeImpactIntensity);
	CurrentImpactShaderStep++;

	//UE_LOG(GP_AICharacterLog, Display, TEXT("CurrentStep: %d"), CurrentImpactShaderStep);

	if (CurrentImpactShaderStep > StepAmount)
	{
		GetWorld()->GetTimerManager().ClearTimer(HitImpactShaderTimerHandle);
	}
}

void AGP_AICharacter::HandleQTEAttack()
{
	if(QTETarget)
	{
		QTETarget->TakeDamage(DamageAmount, FDamageEvent{}, Controller, this);
		//UE_LOG(GP_AICharacterLog, Display, TEXT("HandleQTEAttack"));
	}
	else
	{
		//UE_LOG(GP_AICharacterLog, Display, TEXT("no QTETarget"));
	}
}

void AGP_AICharacter::HandleAnimBlendOut(UAnimMontage* AnimMontage, FName InSectionName, float BlendOutStartTime)
{
	GetWorld()->GetTimerManager().ClearTimer(AnimationTimerHandle);
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && AnimMontage && AnimMontage->IsValidSectionName(InSectionName))
	{
		float SectionLength = GetMontageSectionLength(AnimMontage, InSectionName);
		float BlendOutTime = SectionLength - BlendOutStartTime;
		UE_LOG(GP_AICharacterLog, Display, TEXT("Section %s Length = %f"), *InSectionName.ToString(), SectionLength);

		FAlphaBlendArgs AlphaBlendArgs;
		AlphaBlendArgs.BlendTime = BlendOutTime;
		AlphaBlendArgs.BlendOption = EAlphaBlendOption::CircularOut;

		FMontageBlendSettings MontageBlendSettings(AlphaBlendArgs);

		AnimInstance->Montage_StopWithBlendSettings(MontageBlendSettings, AnimMontage);
		UE_LOG(GP_AICharacterLog, Display, TEXT("BlendOutTime = %f for Section %s"), BlendOutTime, *InSectionName.ToString());
	}
}

float AGP_AICharacter::GetMontageSectionLength(UAnimMontage* AnimMontage, FName InSectionName)
{
	if (AnimMontage && AnimMontage->IsValidSectionName(InSectionName))
	{
		const int32 SectionIndex = AnimMontage->GetSectionIndex(InSectionName);
		if (SectionIndex != INDEX_NONE)
		{
			return AnimMontage->GetSectionLength(SectionIndex);
		}
	}
	return 0.0f;
}

void AGP_AICharacter::HandleNextAnimation(FName InSectionName)
{
	//UE_LOG(GP_AICharacterLog, Display, TEXT("HandleNextAnimation"));

	GetWorld()->GetTimerManager().ClearTimer(AnimationTimerHandle);
	if (AttackAnimMontage)
	{
		UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->Montage_JumpToSection(InSectionName, AttackAnimMontage);
		}
	}
}
