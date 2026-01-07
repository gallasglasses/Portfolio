// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GP_HealthComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnHealthChangedSignature, float, Health, float, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeathSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnDamageSignature, float, Damage, const UDamageType*, DamageType, AController*, InstigatedBy);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GP3_TEAM10_API UGP_HealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UGP_HealthComponent();

	UPROPERTY(BlueprintAssignable)
	FOnHealthChangedSignature OnHealthChanged;

	UPROPERTY(BlueprintAssignable)
	FOnDeathSignature OnDeath;

	UPROPERTY(BlueprintAssignable)
	FOnDamageSignature OnDamage;

	UFUNCTION(BlueprintCallable, Category = "Health")
	bool IsDead() const { return FMath::IsNearlyZero(CurrentHealth); }

	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetHealthPercent() const { return CurrentHealth / MaxHealth; }

	UFUNCTION(BlueprintCallable, Category = "Health")
	float GetHealth() const { return CurrentHealth; }

	UFUNCTION(BlueprintCallable, Category = "Health")
	bool TryToAddHealth(float HealthAmount);

	UFUNCTION(BlueprintCallable, Category = "Health")
	bool IsHealthFull() const;

	UFUNCTION(BlueprintCallable, Category = "Health")
	bool IsImmortal() const { return CanBeImmortal; }

	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetIsImmortal(bool isTrue);

protected:

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health", meta = (ClampMin = "0", ClampMax = "1000.0"))
	float MaxHealth = 100.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Health", meta = (ClampMin = "0", ClampMax = "1000.0"))
	float StartCurrentHealth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health|Immortality")
	bool CanBeImmortal = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Heal", meta = (ToolTip = "Just for testing"))
	bool AutoHeal = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Heal", meta = (EditCondition = "AutoHeal", ToolTip = "Just for testing"))
	float HealUpdateTime = 1.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Heal", meta = (EditCondition = "AutoHeal", ToolTip = "Just for testing"))
	float HealDelay = 5.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Heal", meta = (EditCondition = "AutoHeal", ToolTip = "Just for testing"))
	float HealModifier = 5.f;

	virtual void BeginPlay() override;

private:

	FTimerHandle HealTimerHandle;

	float CurrentHealth = 0.0f;
	bool bDamageHandled = false;

	UFUNCTION(BlueprintCallable)
	void OnTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION(BlueprintCallable)
	void OnTakePointDamage(AActor* DamagedActor, float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser);

	void HealUpdate();

	UFUNCTION(BlueprintCallable)
	void SetHealth(float NewHealth);

	void HandleDamage(float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);
};
