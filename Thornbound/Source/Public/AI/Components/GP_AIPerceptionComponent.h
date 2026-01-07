// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AIPerceptionComponent.h"
#include "GP_AIPerceptionComponent.generated.h"



UCLASS()
class GP3_TEAM10_API UGP_AIPerceptionComponent : public UAIPerceptionComponent
{
	GENERATED_BODY()

public:

	AActor* GetTargetEnemy() const;
	
};
