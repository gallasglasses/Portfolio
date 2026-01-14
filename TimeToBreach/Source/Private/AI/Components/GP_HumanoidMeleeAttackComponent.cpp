// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Components/GP_HumanoidMeleeAttackComponent.h"
#include "AI/Components/GP_AIMeleeAttackComponent.h"
#include "../../../GameModes/GameplayGamemode.h"

DEFINE_LOG_CATEGORY_STATIC(GP_HumanoidMeleeAttackComponentLog, All, All);

void UGP_HumanoidMeleeAttackComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!GetWorld()) return;
}

UGP_HumanoidMeleeAttackComponent::UGP_HumanoidMeleeAttackComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UGP_HumanoidMeleeAttackComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{

	if (bOnCooldownAfterSecondMeleeAttack)
	{
		if (ElapsedCooldownTime >= SecondMeleeCooldown)
		{
			ResetCooldown();
		}
		else
		{
			ElapsedCooldownTime += DeltaTime * TimeDilation;

			//UE_LOG(GP_HumanoidMeleeAttackComponentLog, Display, TEXT("ElapsedCooldownTime = %f | SecondMeleeCooldown = %f | TimeDilation = %f"), ElapsedCooldownTime , SecondMeleeCooldown , TimeDilation);
		}
	}

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UGP_HumanoidMeleeAttackComponent::PerformMeleeAttack_Implementation(AActor* InstigatorActor, AActor* Target)
{
	if (!CanPerformMeleeAttack_Internal() || !Target) return;

	bIsDamageDone = false;
	bOnMeleeAttack = true;
}

bool UGP_HumanoidMeleeAttackComponent::CanPerformMeleeAttack_Internal() const
{
	return !bOnSecondMeleeAttack && !bOnMeleeAttack && !bOnCooldownAfterMeleeAttack && !bOnCooldownAfterSecondMeleeAttack;
}

void UGP_HumanoidMeleeAttackComponent::PerformSecondMeleeAttack_Implementation(AActor* InstigatorActor, AActor* Target)
{
	if (!CanPerformMeleeAttack_Internal() || !Target) return;

	bIsDamageDone = false;
	bOnSecondMeleeAttack = true;
}

void UGP_HumanoidMeleeAttackComponent::ApplySecondMeleeAttackDamage_Implementation(AActor* Target, AActor* Instigator)
{
}

FOnAISecondMeleeAttackFinishedSignature& UGP_HumanoidMeleeAttackComponent::GetAISecondMeleeAttackFinishedDelegate()
{
	return OnSecondMeleeAttackFinished;
}

void UGP_HumanoidMeleeAttackComponent::StopPerformSecondMeleeAttack()
{
	bOnCooldownAfterSecondMeleeAttack = true;
	bOnSecondMeleeAttack = false;
	OnSecondMeleeAttackFinished.Broadcast();
}

void UGP_HumanoidMeleeAttackComponent::ResetCooldown()
{
	if (bOnCooldownAfterMeleeAttack)
	{
		bOnCooldownAfterMeleeAttack = false;
	}
	if (bOnCooldownAfterSecondMeleeAttack)
	{
		bOnCooldownAfterSecondMeleeAttack = false;
	}
	ElapsedCooldownTime = 0.f;
	bIsDamageDone = false;
}
