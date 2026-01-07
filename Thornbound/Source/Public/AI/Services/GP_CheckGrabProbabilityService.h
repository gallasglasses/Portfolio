// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "GP_CheckGrabProbabilityService.generated.h"



UCLASS()
class GP3_TEAM10_API UGP_CheckGrabProbabilityService : public UBTService
{
	GENERATED_BODY()

public:

	UGP_CheckGrabProbabilityService();

protected:

	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	FBlackboardKeySelector IsProbablyGrabAttackKey;

	UPROPERTY()
	FRandomStream GrabRandomStream;
};
