// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/GP_HealthComponent.h"
#include "GameFramework/Character.h"

DEFINE_LOG_CATEGORY_STATIC(GP_HealthComponentLog, All, All);

UGP_HealthComponent::UGP_HealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}


void UGP_HealthComponent::BeginPlay()
{
	Super::BeginPlay();

	check(MaxHealth > 0);
	
	Health = MaxHealth;
	OnHealthChanged.Broadcast(Health, Health);
}

void UGP_HealthComponent::SetCurrentHealth(float CurrentHealth)
{
	const auto NextHealth = FMath::Clamp(CurrentHealth, 0.0f, MaxHealth);
	const auto PreviousHealth = Health;
	Health = NextHealth;

	OnHealthChanged.Broadcast(PreviousHealth, NextHealth);
}

void UGP_HealthComponent::RewindCurrentHealth(float NewHealth)
{
	const float NextHealth = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
	const float PreviousHealth = Health;
	
	Health = NextHealth;
	OnHealthChanged.Broadcast(PreviousHealth, NextHealth);
}

bool UGP_HealthComponent::IsHealthFull() const
{
	return FMath::IsNearlyEqual(Health, MaxHealth);
}

bool UGP_HealthComponent::TryToAddHealth(float HealthAmount)
{
	if (IsDead() || IsHealthFull() || HealthAmount <= 0) return false;
	SetCurrentHealth(Health + HealthAmount);
	return true;
}

