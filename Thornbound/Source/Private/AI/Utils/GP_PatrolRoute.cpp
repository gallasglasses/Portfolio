// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Utils/GP_PatrolRoute.h"
#include "Components/SplineComponent.h"

DEFINE_LOG_CATEGORY_STATIC(GP_PatrolRouteLog, All, All);

AGP_PatrolRoute::AGP_PatrolRoute()
{
	PrimaryActorTick.bCanEverTick = false;

	SplineComponent = CreateDefaultSubobject<USplineComponent>("SplineComponent");
	SplineComponent->SetupAttachment(GetRootComponent());
}

void AGP_PatrolRoute::BeginPlay()
{
	Super::BeginPlay();

	check(SplineComponent);
}

void AGP_PatrolRoute::IncrementPatrolPath()
{
	const int32 NumPoints = SplineComponent->GetNumberOfSplinePoints();
	if (NumPoints == 0)
	{
		UE_LOG(GP_PatrolRouteLog, Warning, TEXT("Spline has no points!"));
		return;
	}

	if (SplineComponent->IsClosedLoop())
	{
		PatrolIndex = (PatrolIndex + 1) % NumPoints;
	}
	else
	{
		PatrolIndex += CurrentDirection;
		if (PatrolIndex >= NumPoints - 1)
		{
			PatrolIndex = NumPoints - 1;
			CurrentDirection = PatrolDirection.Y;
		}
		else if (PatrolIndex <= 0)
		{
			PatrolIndex = 0;
			CurrentDirection = PatrolDirection.X;
		}
	}
}

FVector AGP_PatrolRoute::GetSplinePointAsWorldPosition()
{
	const int32 NumPoints = SplineComponent->GetNumberOfSplinePoints();
	if (NumPoints == 0 || PatrolIndex < 0 || PatrolIndex >= NumPoints)
	{
		UE_LOG(GP_PatrolRouteLog, Warning, TEXT("Invalid PatrolIndex: %d"), PatrolIndex);
		return FVector::ZeroVector;
	}

	return SplineComponent->GetLocationAtSplinePoint(PatrolIndex, ESplineCoordinateSpace::World);
}