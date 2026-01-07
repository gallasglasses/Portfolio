// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GP_AIController.generated.h"

class UGP_AIPerceptionComponent;

UCLASS()
class GP3_TEAM10_API AGP_AIController : public AAIController
{
	GENERATED_BODY()

public:

	AGP_AIController();

	virtual ETeamAttitude::Type GetTeamAttitudeTowards(const AActor& Other) const override;

	UFUNCTION(BlueprintCallable, Category = "Generic Team Id")
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override;

	UFUNCTION(BlueprintCallable, Category = "Generic Team Id")
	virtual FGenericTeamId GetGenericTeamId() const override;

	UFUNCTION()
	void HandleDamage(float Damage, const UDamageType* DamageType, AController* InstigatedBy);

	int32 GetReservedAttackTokens() const { return ReservedAttackTokens; }
	void SetReservedAttackTokens(int32 TokenAmount);

protected:

	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UGP_AIPerceptionComponent* AIPerceptionComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FName FocusOnKeyName = "TargetActor";

private:

	AActor* GetFocusOnActor() const;

	int32 ReservedAttackTokens = 0;
};
