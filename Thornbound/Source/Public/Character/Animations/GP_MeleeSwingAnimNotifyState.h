// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GP_MeleeSwingAnimNotifyState.generated.h"



UCLASS()
class GP3_TEAM10_API UGP_MeleeSwingAnimNotifyState : public UAnimNotifyState
{
	GENERATED_BODY()

public:

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeleeSwing")
	FName SocketName = "hand_r";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeleeSwing")
	float Radius = 15.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeleeSwing")
	float MeleeSwingDamage = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MeleeSwing")
	TEnumAsByte<ECollisionChannel> CollisionChannel = ECC_Pawn;

private:

	FVector PreviousHandLocation;
	FVector CurrentHandLocation;

	bool bIsDamageDone = false;
	
};
