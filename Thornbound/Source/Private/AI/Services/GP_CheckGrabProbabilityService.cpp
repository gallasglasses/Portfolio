// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/GP_CheckGrabProbabilityService.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AIController.h"
#include "Kismet/KismetMathLibrary.h"
#include "AI/GP_AICharacter.h"

DEFINE_LOG_CATEGORY_STATIC(GP_CheckGrabProbabilityServiceLog, All, All);

UGP_CheckGrabProbabilityService::UGP_CheckGrabProbabilityService()
{
	NodeName = "Check Grab Probability";

	Interval = 1.f;

	GrabRandomStream.Initialize(FDateTime::Now().GetTicks() % INT32_MAX);
}

void UGP_CheckGrabProbabilityService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	const auto Controller = OwnerComp.GetAIOwner();
	const auto Blackboard = OwnerComp.GetBlackboardComponent();
	if (!Controller || !Blackboard) return;

	const auto AICharacter = Cast<AGP_AICharacter>(Controller->GetPawn());
	if (!AICharacter) return;

	const float GrabProbability = AICharacter->GetGrabProbability();

	float ProbabilityNormalized = GrabProbability / 100.0f;
	GrabRandomStream.GenerateNewSeed();
	float RandValue = GrabRandomStream.FRand();
	bool IsProbablyGrabAttack = RandValue < ProbabilityNormalized;

	Blackboard->SetValueAsBool(IsProbablyGrabAttackKey.SelectedKeyName, IsProbablyGrabAttack);
	UE_LOG(GP_CheckGrabProbabilityServiceLog, Display, TEXT("GrabProbability = %.2f, Rand = %.2f, Result = %s"), GrabProbability, RandValue, IsProbablyGrabAttack ? TEXT("true") : TEXT("false"));
}