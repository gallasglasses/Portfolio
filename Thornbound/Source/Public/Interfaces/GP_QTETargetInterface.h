// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GP_QTETargetInterface.generated.h"

UINTERFACE(MinimalAPI)
class UGP_QTETargetInterface : public UInterface
{
	GENERATED_BODY()
};



class GP3_TEAM10_API IGP_QTETargetInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void StartQTE(AActor* InstigatorActor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void FinishQTE();

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	FTransform GetAttackPoint() const;

	UFUNCTION(BlueprintImplementableEvent, Category = "QTE")
	void OnQTEProgressUpdated(float Progress, float TimeLeft);
};
