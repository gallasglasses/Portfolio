// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "GP_IsAwakeningDecorator.generated.h"



UCLASS()
class GP3_TEAM10_API UGP_IsAwakeningDecorator : public UBTDecorator
{
	GENERATED_BODY()

public:

	UGP_IsAwakeningDecorator();

protected:

	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;
	
};
