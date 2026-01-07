// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/GP_FindEnemyService.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "AI/Components/GP_AIPerceptionComponent.h"

UGP_FindEnemyService::UGP_FindEnemyService()
{
	NodeName = "Find Enemy";
}

void UGP_FindEnemyService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	const auto Blackboard = OwnerComp.GetBlackboardComponent();
	if (Blackboard)
	{
		const auto Controller = OwnerComp.GetAIOwner();
		if (Controller)
		{
			const auto PerceptionComponent = Controller->FindComponentByClass<UGP_AIPerceptionComponent>();
			if (PerceptionComponent && !Blackboard->GetValueAsBool(IsFlowerTotemEventKey.SelectedKeyName))
			{
				Blackboard->SetValueAsObject(TargetActorKey.SelectedKeyName, PerceptionComponent->GetTargetEnemy());
			}
		}
	}
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}


