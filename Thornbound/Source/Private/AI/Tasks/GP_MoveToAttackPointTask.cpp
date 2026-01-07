// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/GP_MoveToAttackPointTask.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Interfaces/GP_QTETargetInterface.h"
#include "GameFramework/Actor.h"

UGP_MoveToAttackPointTask::UGP_MoveToAttackPointTask()
{
	NodeName = "MoveToAttackPoint";
}

EBTNodeResult::Type UGP_MoveToAttackPointTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const auto Controller = OwnerComp.GetAIOwner();
	const auto Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Controller || !Blackboard)
	{
		return EBTNodeResult::Failed;
	}

	AActor* Player = Cast<AActor>(Blackboard->GetValueAsObject(TargetActorKey.SelectedKeyName));

	if (!Player && !Player->GetClass()->ImplementsInterface(UGP_QTETargetInterface::StaticClass()))
	{
		return EBTNodeResult::Failed;
	}

	const FTransform AttackPoint = IGP_QTETargetInterface::Execute_GetAttackPoint(Player);
	FVector Destination = AttackPoint.GetLocation();

	Blackboard ->SetValueAsVector(TargetLocationKey.SelectedKeyName, Destination);

	return EBTNodeResult::Succeeded;
}