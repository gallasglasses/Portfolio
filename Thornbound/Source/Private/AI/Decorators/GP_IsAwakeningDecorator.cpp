// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Decorators/GP_IsAwakeningDecorator.h"
#include "AIController.h"
#include "AI/GP_AICharacter.h"

DEFINE_LOG_CATEGORY_STATIC(GP_IsAwakeningDecoratorLog, All, All);

UGP_IsAwakeningDecorator::UGP_IsAwakeningDecorator()
{
	NodeName = "Is Awakening";
}

bool UGP_IsAwakeningDecorator::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	const auto Controller = OwnerComp.GetAIOwner();
	if (!Controller) return false;

	const auto AIPawn = Cast<AGP_AICharacter>(Controller->GetPawn());
	if (!AIPawn) return false;

	bool bCanAwake = AIPawn->CanAwake() && !AIPawn->IsAwakening() && AIPawn->GetCurrentAnimState() == EAIAnimState::Sleep;

	//UE_LOG(GP_IsAwakeningDecoratorLog, Display, TEXT("AIPawn = %s, bCanAwake = %s"), *AIPawn->GetName(), bCanAwake ? TEXT("TRUE") : TEXT("FALSE"));
	return bCanAwake;
}

