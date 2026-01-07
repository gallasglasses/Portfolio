// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Animations/GP_FinishAwakeningAnimNotify.h"
#include "AI/GP_AICharacter.h"

void UGP_FinishAwakeningAnimNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	if (!MeshComp) return;

	AActor* Owner = MeshComp->GetOwner();
	if (!Owner) return;

	if (AGP_AICharacter* AICharacter = Cast<AGP_AICharacter>(Owner))
	{
		AICharacter->FinishAwake();
	}
}