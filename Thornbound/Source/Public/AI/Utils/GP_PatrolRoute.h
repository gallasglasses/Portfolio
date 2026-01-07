// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GP_PatrolRoute.generated.h"

class USplineComponent;

UCLASS()
class GP3_TEAM10_API AGP_PatrolRoute : public AActor
{
	GENERATED_BODY()

public:

	AGP_PatrolRoute();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Components")
	USplineComponent* SplineComponent;

public:

	UFUNCTION(BlueprintCallable, Category = "AI|Patrol")
	void IncrementPatrolPath();

	UFUNCTION(BlueprintCallable, Category = "AI|Patrol")
	FVector GetSplinePointAsWorldPosition();

private:

	int32 PatrolIndex = 0;
	const FIntPoint PatrolDirection = FIntPoint(1, -1);
	int8 CurrentDirection = 1;
};
