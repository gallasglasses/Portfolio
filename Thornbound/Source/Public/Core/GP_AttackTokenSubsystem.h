// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GP_AttackTokenSubsystem.generated.h"

class AAIController;

//DECLARE_MULTICAST_DELEGATE_OneParam(FOnTokenReservedSignature, AAIController*);

UCLASS()
class GP3_TEAM10_API UGP_AttackTokenSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	UPROPERTY(EditAnywhere, Category = "AttackToken")
	int32 MaxConcurrentAttackers = 1;

	bool RequestToken(AAIController* Requestor, int32 RequestedTokenAmount);

	void ReleaseToken(AAIController* Holder, int32 ReturnedTokenAmount);
	
	//FOnTokenReservedSignature OnTokenReserved;

private:

	//AAIController* ActiveHolder;

	int32 CurrentTokenAmount;

	void SetTokenAmount(int32 NewAmount);
	
};
