// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/GP_HealthComponent.h"
#include "GameFramework/Actor.h"
#include "TimerManager.h"

DEFINE_LOG_CATEGORY_STATIC(HealthComponentLog, All, All);

UGP_HealthComponent::UGP_HealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

bool UGP_HealthComponent::TryToAddHealth(float HealthAmount)
{
	// avoid situations to add health after death or don't waste medkit
	if (IsDead() || IsHealthFull())
	{
		return false;
	}
	SetHealth(CurrentHealth + HealthAmount);
	return true;
}

bool UGP_HealthComponent::IsHealthFull() const
{
	return FMath::IsNearlyEqual(CurrentHealth, MaxHealth);
}

void UGP_HealthComponent::SetIsImmortal(bool isTrue)
{
	UE_LOG(HealthComponentLog, Display, TEXT("CanBeImmortal = %s"), isTrue ? TEXT("TRUE") : TEXT("FALSE"));
	CanBeImmortal = isTrue;
}

void UGP_HealthComponent::BeginPlay()
{
	Super::BeginPlay();

	check(MaxHealth > 0);
	check(StartCurrentHealth > 0);

	SetHealth(StartCurrentHealth);

	AActor* ComponentOwner = GetOwner();
	if (ComponentOwner)
	{
		UE_LOG(HealthComponentLog, Display, TEXT("Owner %s"), *ComponentOwner->GetName());
		ComponentOwner->OnTakeAnyDamage.AddDynamic(this, &UGP_HealthComponent::OnTakeAnyDamage);
		ComponentOwner->OnTakePointDamage.AddDynamic(this, &UGP_HealthComponent::OnTakePointDamage);
	}
}

void UGP_HealthComponent::OnTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	UE_LOG(HealthComponentLog, Display, TEXT("OnTakeAnyDamage"));

	if (bDamageHandled)
	{
		bDamageHandled = false;
		return;
	}

	HandleDamage(Damage, DamageType, InstigatedBy, DamageCauser);
	//if (Damage <= 0.f || IsDead() || !GetWorld() || !DamagedActor)
	//{
	//	UE_LOG(HealthComponentLog, Display, TEXT("(Damage <= 0 || IsDead() || !GetWorld() || !DamagedActor"));
	//	return;
	//}

	//OnDamage.Broadcast(Damage, DamageType, InstigatedBy); // for change animation or stop some interaction

	//UE_LOG(HealthComponentLog, Display, TEXT("Damage: %f"), Damage);
	//SetHealth(CurrentHealth - Damage);

	//GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle); // autoheal - for test

	//if (IsDead())
	//{
	//	OnDeath.Broadcast();
	//}
	//else if (AutoHeal)  // autoheal - for test
	//{
	//	GetWorld()->GetTimerManager().SetTimer(HealTimerHandle, this, &UGP_HealthComponent::HealUpdate, HealUpdateTime, true, HealDelay);
	//}
}

void UGP_HealthComponent::OnTakePointDamage(AActor* DamagedActor, float Damage, AController* InstigatedBy, FVector HitLocation, UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const UDamageType* DamageType, AActor* DamageCauser)
{
	UE_LOG(HealthComponentLog, Display, TEXT("OnTakePointDamage: %s"), *BoneName.ToString());

	/*bool bIsHeadshot = BoneName == "Bip01-Head";
	if (bIsHeadshot)
	{
		Damage = MAX_FLT;
		UE_LOG(HealthComponentLog, Display, TEXT("OnTakePointDamage: Headshot!"));
	}*/

	bDamageHandled = true;
	HandleDamage(Damage, DamageType, InstigatedBy, DamageCauser);
}

void UGP_HealthComponent::HealUpdate()  // autoheal - for test
{
	SetHealth(CurrentHealth + HealModifier);

	if (FMath::IsNearlyEqual(CurrentHealth, MaxHealth) && GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle);
	}
}

void UGP_HealthComponent::SetHealth(float NewHealth)
{
	const auto NextHealth = FMath::Clamp(NewHealth, 0.0f, MaxHealth);
	const auto HealthDelta = NextHealth - CurrentHealth;

	UE_LOG(HealthComponentLog, Display, TEXT("Delta: %f | CurrentHealth: %f | NextHealth: %f"), HealthDelta, CurrentHealth, NextHealth);

	CurrentHealth = NextHealth;
	OnHealthChanged.Broadcast(CurrentHealth, HealthDelta);
}

void UGP_HealthComponent::HandleDamage(float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	if (Damage <= 0.f || IsDead() || !GetWorld())
	{
		UE_LOG(HealthComponentLog, Display, TEXT("(Damage <= 0 || IsDead() || !GetWorld()"));
		return;
	}

	OnDamage.Broadcast(Damage, DamageType, InstigatedBy); // for change animation or stop some interaction

	if (!CanBeImmortal)
	{
		UE_LOG(HealthComponentLog, Display, TEXT("Damage: %f"), Damage);
		SetHealth(CurrentHealth - Damage);
	}

	GetWorld()->GetTimerManager().ClearTimer(HealTimerHandle); // autoheal - for test

	if (IsDead())
	{
		OnDeath.Broadcast();
	}
	else if (AutoHeal)  // autoheal - for test
	{
		GetWorld()->GetTimerManager().SetTimer(HealTimerHandle, this, &UGP_HealthComponent::HealUpdate, HealUpdateTime, true, HealDelay);
	}
}

