// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/GP_CheckDistanceService.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(GP_CheckDistanceServiceLog, All, All);

UGP_CheckDistanceService::UGP_CheckDistanceService()
{
	NodeName = "Check Distance";

	Interval = 0.1f;
}

void UGP_CheckDistanceService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const auto Controller = OwnerComp.GetAIOwner();
	const auto Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Controller || !Blackboard) return;

	const auto AIPawn = Controller->GetPawn();
	if (!AIPawn) return;

	AActor* Target = Cast<AActor>(Blackboard->GetValueAsObject(TargetActorKey.SelectedKeyName));
	if (!Target)
	{
		Blackboard->SetValueAsBool(IsCloseEnoughKey.SelectedKeyName, false);
		return;
	}

	const float Distance = FVector::Dist(Target->GetActorLocation(), AIPawn->GetActorLocation());

	bool IsCloseEnough = UKismetMathLibrary::InRange_FloatFloat(Distance, 0.f, RequiredDistance + AcceptableRadius, false, true);

	Blackboard->SetValueAsBool(IsCloseEnoughKey.SelectedKeyName, IsCloseEnough);

	//UE_LOG(GP_CheckDistanceServiceLog, Display, TEXT("Distance between Target and AIEnenmy = %f, CloseEnough = %s"), Distance, IsCloseEnough ? TEXT("true") : TEXT("false"));
}
