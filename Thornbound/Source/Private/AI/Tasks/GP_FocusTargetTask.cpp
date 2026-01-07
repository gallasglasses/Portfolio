// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/GP_FocusTargetTask.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"

DEFINE_LOG_CATEGORY_STATIC(GP_FocusTargetTaskLog, All, All);

UGP_FocusTargetTask::UGP_FocusTargetTask()
{
	NodeName = "FocusTarget";
}

EBTNodeResult::Type UGP_FocusTargetTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const auto Controller = OwnerComp.GetAIOwner();
	const auto Blackboard = OwnerComp.GetBlackboardComponent();

	if (!Controller || !Blackboard)
	{
		UE_LOG(GP_FocusTargetTaskLog, Display, TEXT("!Controller || !Blackboard"));
		return EBTNodeResult::Failed;
	}

	const auto HasTarget = Blackboard && Blackboard->GetValueAsObject(TargetActorKey.SelectedKeyName);

	if (HasTarget)
	{
		Controller->SetFocus(Cast<AActor>(Blackboard->GetValueAsObject(TargetActorKey.SelectedKeyName)));
		return EBTNodeResult::Succeeded;
	}

	return EBTNodeResult::Failed;
}