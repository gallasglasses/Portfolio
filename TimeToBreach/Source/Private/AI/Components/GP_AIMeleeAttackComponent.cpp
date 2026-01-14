// Galla


#include "AI/Components/GP_AIMeleeAttackComponent.h"
#include "../../../GameModes/GameplayGamemode.h"

DEFINE_LOG_CATEGORY_STATIC(GP_AIMeleeAttackComponentLog, All, All);

UGP_AIMeleeAttackComponent::UGP_AIMeleeAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGP_AIMeleeAttackComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UGP_AIMeleeAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (bOnCooldownAfterMeleeAttack)
	{
		if (ElapsedCooldownTime >= Cooldown)
		{
			ResetCooldown();
		}
		else
		{
			ElapsedCooldownTime += DeltaTime * TimeDilation;

			//UE_LOG(GP_AIMeleeAttackComponentLog, Display, TEXT("ElapsedCooldownTime = %f | Cooldown = %f | TimeDilation = %f"), ElapsedCooldownTime / TimeDilation, Cooldown / TimeDilation, TimeDilation);
		}
	}
}

bool UGP_AIMeleeAttackComponent::CanPerformMeleeAttack_Internal() const
{
	return !bOnMeleeAttack && !bOnCooldownAfterMeleeAttack;
}

bool UGP_AIMeleeAttackComponent::CanPerformMeleeAttack()
{
	return CanPerformMeleeAttack_Internal();
}

void UGP_AIMeleeAttackComponent::PerformMeleeAttack_Implementation(AActor* InstigatorActor, AActor* Target)
{
	if (!CanPerformMeleeAttack_Internal() || !Target) return;

	UE_LOG(GP_AIMeleeAttackComponentLog, Display, TEXT("PerformMeleeAttack"));

	bIsDamageDone = false;
	bOnMeleeAttack = true;
}

void UGP_AIMeleeAttackComponent::StopPerformMeleeAttack()
{
	bOnCooldownAfterMeleeAttack = true;
	bOnMeleeAttack = false;
	OnMeleeAttackFinished.Broadcast();
	UE_LOG(GP_AIMeleeAttackComponentLog, Display, TEXT("StopPerformMeleeAttack"));
}

void UGP_AIMeleeAttackComponent::ApplyMeleeAttackDamage_Implementation(AActor* Target, AActor* Instigator)
{
}

FOnAIMeleeAttackFinishedSignature& UGP_AIMeleeAttackComponent::GetAIMeleeAttackFinishedDelegate()
{
	return OnMeleeAttackFinished;
}

void UGP_AIMeleeAttackComponent::OnStartGlobalNormalTime()
{
	TimeDilation = 1.0f;
	//UE_LOG(GP_AIMeleeAttackComponentLog, Display, TEXT("OnStartGlobalNormalTime TimeDilation = %f"), TimeDilation);
}

void UGP_AIMeleeAttackComponent::OnStopGlobalNormalTime(float Time)
{
	TimeDilation = Time;
	//UE_LOG(GP_AIMeleeAttackComponentLog, Display, TEXT("OnStopGlobalNormalTime TimeDilation = %f"), TimeDilation);
}

void UGP_AIMeleeAttackComponent::ResetCooldown()
{
	if (bOnCooldownAfterMeleeAttack)
	{
		bOnCooldownAfterMeleeAttack = false;
	}
	ElapsedCooldownTime = 0.f;
	bIsDamageDone = false;

	//UE_LOG(GP_AIMeleeAttackComponentLog, Display, TEXT("ResetCooldown"));
}