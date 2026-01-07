// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/GP_AttackTask.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "AI/GP_AICharacter.h"
#include "Core/GP_AttackTokenSubsystem.h"

DEFINE_LOG_CATEGORY_STATIC(GP_AttackTaskLog, All, All);

UGP_AttackTask::UGP_AttackTask()
{
	NodeName = "Attack";
}

EBTNodeResult::Type UGP_AttackTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	const auto Controller = OwnerComp.GetAIOwner();
	const auto Blackboard = OwnerComp.GetBlackboardComponent();

	if (!Controller || !Blackboard)
	{
		UE_LOG(GP_AttackTaskLog, Display, TEXT("!Controller || !Blackboard"));
		return EBTNodeResult::Failed;
	}

	const auto Owner = Cast<AGP_AICharacter>(Controller->GetPawn());
	if (!Owner)
	{
		UE_LOG(GP_AttackTaskLog, Display, TEXT("!Owner"));
		return EBTNodeResult::Failed;
	}

	const auto HasTarget = Blackboard && Blackboard->GetValueAsObject(TargetActorKey.SelectedKeyName) && Blackboard->GetValueAsBool(IsCloseEnoughKey.SelectedKeyName);
	if (HasTarget)
	{
		UE_LOG(GP_AttackTaskLog, Display, TEXT("HasTarget"));


		UGP_AttackTokenSubsystem* AttackTokenSubsystem = GetWorld()->GetSubsystem<UGP_AttackTokenSubsystem>();
		if (!AttackTokenSubsystem)
		{
			UE_LOG(GP_AttackTaskLog, Error, TEXT("No AttackTokenSubsystem in world"));
			return EBTNodeResult::Failed;
		}

		if (AttackTokenSubsystem->RequestToken(Controller, 1))
		{
			Blackboard->SetValueAsBool(IsAttackingKey.SelectedKeyName, true);
			Owner->Attack();
			UE_LOG(GP_AttackTaskLog, Display, TEXT("Attack: Controller = %s, IsAttackingKey = %s"), *Controller->GetName(), Blackboard->GetValueAsBool(IsAttackingKey.SelectedKeyName) ? TEXT("TRUE") : TEXT("FALSE"));

			if (Owner->OnFinishedAttack.IsBound())
			{
				Owner->OnFinishedAttack.Clear();
			}

			UBehaviorTreeComponent* CapturedComp = &OwnerComp;
			AAIController* CapturedController = Controller;

			Owner->OnFinishedAttack.AddLambda(
				[this, CapturedComp, CapturedController]()
				{
					if (!CapturedComp || !CapturedController) return;
					const auto MemoryBlackboard = CapturedComp->GetBlackboardComponent();
					if (!MemoryBlackboard)
					{
						FinishLatentTask(*CapturedComp, EBTNodeResult::Failed);
						return;
					}

					UGP_AttackTokenSubsystem* AttackTokenSubsystem = GetWorld()->GetSubsystem<UGP_AttackTokenSubsystem>();
					if (!AttackTokenSubsystem)
					{
						FinishLatentTask(*CapturedComp, EBTNodeResult::Failed);
						return;
					}

					AttackTokenSubsystem->ReleaseToken(CapturedController, 1);

					AGP_AICharacter* Owner = Cast<AGP_AICharacter>(CapturedController->GetPawn());
					Owner->SetCurrentAnimState(EAIAnimState::Idle);

					MemoryBlackboard->SetValueAsBool(IsAttackingKey.SelectedKeyName, false);

					UE_LOG(GP_AttackTaskLog, Display, TEXT("Finish Attack: Controller = %s, IsAttackingKey = %s"), *CapturedController->GetName(), MemoryBlackboard->GetValueAsBool(IsAttackingKey.SelectedKeyName) ? TEXT("TRUE") : TEXT("FALSE"));

					FinishLatentTask(*CapturedComp, EBTNodeResult::Succeeded);
					return;
				}
			);
		}
		else
		{
			UE_LOG(GP_AttackTaskLog, Error, TEXT("Request Token is denied for %s"), *Owner->GetName());
			return EBTNodeResult::Failed;
		}
		
	}
	else
	{
		UE_LOG(GP_AttackTaskLog, Display, TEXT("Has No Target"));
		return EBTNodeResult::Failed;
	}

	return EBTNodeResult::InProgress;
}

EBTNodeResult::Type UGP_AttackTask::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* Controller = OwnerComp.GetAIOwner();
	if (Controller)
	{
		if (auto* Owner = Cast<AGP_AICharacter>(Controller->GetPawn()))
		{
			Owner->OnFinishedAttack.RemoveAll(this);
			if (Controller->BrainComponent->IsPaused())
			{
				Controller->BrainComponent->ResumeLogic("QTE Finished");
			}
		}
	}
	return Super::AbortTask(OwnerComp, NodeMemory);
}
