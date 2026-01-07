// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Animations/GP_StunAnimNotifyState.h"
#include "AI/GP_AICharacter.h"
#include "AI/GP_AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

DEFINE_LOG_CATEGORY_STATIC(GP_StunAnimNotifyStateLog, All, All);

void UGP_StunAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp && !MeshComp->GetOwner()) return;

	AIMeshComponent = MeshComp;
	AGP_AICharacter* AICharacter = Cast<AGP_AICharacter>(MeshComp->GetOwner());
	if (!AICharacter) return;

	AICharacter->OnQTEStarted.AddDynamic(this, &UGP_StunAnimNotifyState::AbortStun);
	AAIController* AIController = Cast<AAIController>(AICharacter->GetController());
	if (AIController)
	{
		UE_LOG(GP_StunAnimNotifyStateLog, Log, TEXT("NotifyBegin for %s"), *AICharacter->GetName());
		UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
		if (BlackboardComp)
		{
			BlackboardComp->SetValueAsBool(IsStunnedKeyName, true);

			UE_LOG(GP_StunAnimNotifyStateLog, Log, TEXT("Set IsStunnedKey to true for %s"), *AICharacter->GetName());
		}
		else
		{
			UE_LOG(GP_StunAnimNotifyStateLog, Warning, TEXT("BlackboardComponent not found for %s"), *AICharacter->GetName());
		}
	}
}

void UGP_StunAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp && !MeshComp->GetOwner()) return;

	AGP_AICharacter* AICharacter = Cast<AGP_AICharacter>(MeshComp->GetOwner());
	if (!AICharacter) return;

	AICharacter->OnQTEStarted.RemoveDynamic(this, &UGP_StunAnimNotifyState::AbortStun);
	AAIController* AIController = Cast<AAIController>(AICharacter->GetController());
	if (AIController)
	{
		UE_LOG(GP_StunAnimNotifyStateLog, Log, TEXT("NotifyEnd for %s"), *AICharacter->GetName());
		UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
		if (BlackboardComp && BlackboardComp->GetValueAsBool(IsStunnedKeyName) == true)
		{
			BlackboardComp->SetValueAsBool(IsStunnedKeyName, false);

			UE_LOG(GP_StunAnimNotifyStateLog, Log, TEXT("Set IsStunnedKey to false for %s"), *AICharacter->GetName());
		}
		else
		{
			UE_LOG(GP_StunAnimNotifyStateLog, Warning, TEXT("BlackboardComponent not found for %s"), *AICharacter->GetName());
		}
	}
}

void UGP_StunAnimNotifyState::AbortStun()
{
	if(!AIMeshComponent) return;

	AGP_AICharacter* AICharacter = Cast<AGP_AICharacter>(AIMeshComponent->GetOwner());
	if (!AICharacter) return;

	AICharacter->OnQTEStarted.RemoveDynamic(this, &UGP_StunAnimNotifyState::AbortStun);
	AAIController* AIController = Cast<AAIController>(AICharacter->GetController());
	if (AIController)
	{
		UE_LOG(GP_StunAnimNotifyStateLog, Log, TEXT("AbortStun for %s"), *AICharacter->GetName());
		UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
		if (BlackboardComp)
		{
			BlackboardComp->SetValueAsBool(IsStunnedKeyName, false);

			UE_LOG(GP_StunAnimNotifyStateLog, Log, TEXT("Set IsStunnedKey to false for %s"), *AICharacter->GetName());
		}
		else
		{
			UE_LOG(GP_StunAnimNotifyStateLog, Warning, TEXT("BlackboardComponent not found for %s"), *AICharacter->GetName());
		}
	}
	AIMeshComponent = nullptr;
}
