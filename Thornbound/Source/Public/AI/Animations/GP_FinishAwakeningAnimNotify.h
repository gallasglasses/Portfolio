// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GP_FinishAwakeningAnimNotify.generated.h"



UCLASS()
class GP3_TEAM10_API UGP_FinishAwakeningAnimNotify : public UAnimNotify
{
	GENERATED_BODY()

public:

	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;

	virtual FString GetNotifyName_Implementation() const override
	{
		return TEXT("Finish Awakening Notify");
	}
	
};
