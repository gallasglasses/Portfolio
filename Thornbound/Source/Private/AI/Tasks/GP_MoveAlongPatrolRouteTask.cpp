// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/GP_MoveAlongPatrolRouteTask.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "AI/GP_AICharacter.h"
#include "AI/Utils/GP_PatrolRoute.h"

DEFINE_LOG_CATEGORY_STATIC(GP_MoveAlongPatrolRouteTaskLog, All, All);

UGP_MoveAlongPatrolRouteTask::UGP_MoveAlongPatrolRouteTask()
{
	NodeName = "Move Along Patrol Route";
}

EBTNodeResult::Type UGP_MoveAlongPatrolRouteTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const auto Controller = OwnerComp.GetAIOwner();
	const auto Blackboard = OwnerComp.GetBlackboardComponent();
	const auto Owner = Cast<AGP_AICharacter>(Controller->GetPawn());

	if (!Controller || !Blackboard || !Owner)
	{
		UE_LOG(GP_MoveAlongPatrolRouteTaskLog, Display, TEXT("Not Valid"));
		return EBTNodeResult::Failed;
	}

	if (IsValid(Owner->PatrolRoute))
	{
		FVector Destination = Owner->PatrolRoute->GetSplinePointAsWorldPosition();

		Blackboard->SetValueAsVector(TargetLocationKey.SelectedKeyName, Destination);

		Owner->PatrolRoute->IncrementPatrolPath();
	}
	else
	{

#if WITH_EDITOR
		//UE_LOG(GP_MoveAlongPatrolRouteTaskLog, Warning, TEXT("PatrolRoute is null!"));
#endif

		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::Succeeded;
}