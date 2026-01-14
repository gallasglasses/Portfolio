// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Components/GP_CrowdFollowingComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/CapsuleComponent.h"
#include "AIController.h"

UGP_CrowdFollowingComponent::UGP_CrowdFollowingComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	/*SetCrowdSeparation(true);
	SetCrowdAnticipateTurns(true);
	SetCrowdSeparationWeight(50.f);
	SetCrowdAvoidanceRangeMultiplier(1.5f);
	SetCrowdAvoidanceQuality(ECrowdAvoidanceQuality::High);*/
}

FVector UGP_CrowdFollowingComponent::GetCrowdAgentLocation() const
{
	if (const AAIController* AIController = Cast<AAIController>(GetOwner()))
	{
		if (const ACharacter* C = Cast<ACharacter>(AIController->GetPawn()))
		{
			return C->GetActorLocation();
		}
	}

	return FVector::ZeroVector;
}

FVector UGP_CrowdFollowingComponent::GetCrowdAgentVelocity() const
{
	if (const AAIController* AIController = Cast<AAIController>(GetOwner()))
	{
		if (const ACharacter* C = Cast<ACharacter>(AIController->GetPawn()))
		{
			if (const UCharacterMovementComponent* MovementComponent = C->GetCharacterMovement())
				return MovementComponent->Velocity;
		}
	}
	return FVector::ZeroVector;
}

void UGP_CrowdFollowingComponent::GetCrowdAgentCollisions(float& CylinderRadius, float& CylinderHalfHeight) const
{
	if (const AAIController* AIController = Cast<AAIController>(GetOwner()))
	{
		if (const ACharacter* C = Cast<ACharacter>(AIController->GetPawn()))
		{
			CylinderRadius = C->GetCapsuleComponent()->GetScaledCapsuleRadius();
			CylinderHalfHeight = C->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
		}
	}
	else
	{
		Super::GetCrowdAgentCollisions(CylinderRadius, CylinderHalfHeight);
	}
}

float UGP_CrowdFollowingComponent::GetCrowdAgentMaxSpeed() const
{
	if (const AAIController* AIController = Cast<AAIController>(GetOwner()))
	{
		if (const ACharacter* C = Cast<ACharacter>(AIController->GetPawn()))
		{
			if (const UCharacterMovementComponent* MovementComponent = C->GetCharacterMovement())
				return MovementComponent->GetMaxSpeed();
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("omg"));
	return 300.f;
}

void UGP_CrowdFollowingComponent::FollowPathSegment(float DeltaTime)
{
	Super::FollowPathSegment(DeltaTime);
}

void UGP_CrowdFollowingComponent::OnPathUpdated()
{
	Super::OnPathUpdated();
}
