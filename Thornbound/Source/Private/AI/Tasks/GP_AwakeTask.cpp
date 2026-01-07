// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/GP_AwakeTask.h"
#include "AIController.h"
#include "AI/GP_AICharacter.h"
#include "Components/GP_HealthComponent.h"

DEFINE_LOG_CATEGORY_STATIC(GP_AwakeTaskLog, All, All);

UGP_AwakeTask::UGP_AwakeTask()
{
	NodeName = "Awake";
}

EBTNodeResult::Type UGP_AwakeTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const auto Controller = OwnerComp.GetAIOwner();
	if (!Controller)
	{
		return EBTNodeResult::Failed;
	}

	const auto Owner = Cast<AGP_AICharacter>(Controller->GetPawn());
	if (!Owner)
	{
		return EBTNodeResult::Failed;
	}

	UE_LOG(GP_AwakeTaskLog, Display, TEXT("AwakeTask"));
	Owner->SetIsAwakening(true);
	Owner->SetCurrentAnimState(EAIAnimState::StandUp);

	UBehaviorTreeComponent* CapturedComp = &OwnerComp;

	Owner->OnFinishedAwake.AddLambda(
		[this, CapturedComp]()
		{
			if (!CapturedComp) return;
			UE_LOG(GP_AwakeTaskLog, Display, TEXT("Finish Awake"));

			AAIController* Controller = CapturedComp->GetAIOwner();
			AGP_AICharacter* Owner = Cast<AGP_AICharacter>(Controller->GetPawn());
			if (!Owner)
			{
				FinishLatentTask(*CapturedComp, EBTNodeResult::Failed);
				return;
			}

			UGP_HealthComponent* Health = Owner->GetComponentByClass<UGP_HealthComponent>();
			if (!Health)
			{
				FinishLatentTask(*CapturedComp, EBTNodeResult::Failed);
				return;
			}
			if (Health->IsDead())
			{
				FinishLatentTask(*CapturedComp, EBTNodeResult::Failed);
				return;
			}

			Owner->SetIsAwakening(false);
			Owner->SetCurrentAnimState(EAIAnimState::Idle);
			FinishLatentTask(*CapturedComp, EBTNodeResult::Succeeded);
		}
	);

	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UGP_AwakeTask::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (Controller)
	{
		if (auto* Owner = Cast<AGP_AICharacter>(Controller->GetPawn()))
		{
			Owner->OnFinishedAwake.RemoveAll(this);
		}
	}
	return Super::AbortTask(OwnerComp, NodeMemory);
}