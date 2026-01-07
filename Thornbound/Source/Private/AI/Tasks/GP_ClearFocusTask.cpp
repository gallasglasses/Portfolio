// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/GP_ClearFocusTask.h"
#include "AIController.h"

DEFINE_LOG_CATEGORY_STATIC(GP_ClearFocusTaskLog, All, All);

UGP_ClearFocusTask::UGP_ClearFocusTask()
{
	NodeName = "ClearFocus";
}

EBTNodeResult::Type UGP_ClearFocusTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const auto Controller = OwnerComp.GetAIOwner();

	if (!Controller)
	{
		UE_LOG(GP_ClearFocusTaskLog, Display, TEXT("!Controller"));
		return EBTNodeResult::Failed;
	}
	else
	{
		Controller->ClearFocus(EAIFocusPriority::Gameplay);
		return EBTNodeResult::Succeeded;
	}
}