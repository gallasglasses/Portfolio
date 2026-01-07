// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/Animations/GP_MeleeSwingAnimNotifyState.h"
#include "Engine/DamageEvents.h"
#include "Character/GP_PlayerCharacter.h"

void UGP_MeleeSwingAnimNotifyState::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp) return;

	PreviousHandLocation = MeshComp->GetSocketLocation(SocketName);
}

void UGP_MeleeSwingAnimNotifyState::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	if (!MeshComp) return;

	AGP_PlayerCharacter* Character = Cast<AGP_PlayerCharacter>(MeshComp->GetOwner());
	if (!Character || bIsDamageDone) return;

	FVector CurrentLocation = MeshComp->GetSocketLocation(SocketName);

	FCollisionQueryParams Params;
	Params.AddIgnoredActor(Character);

	TArray<FHitResult> HitResults;
	bool bHit = MeshComp->GetWorld()->SweepMultiByChannel(
		HitResults,
		PreviousHandLocation,
		CurrentLocation,
		FQuat::Identity,
		CollisionChannel,
		FCollisionShape::MakeSphere(Radius),
		Params
	);

//#if WITH_EDITOR
//		DrawDebugSphere(MeshComp->GetWorld(), PreviousHandLocation, Radius, 12, FColor::Blue, false, 0.2f);
//		DrawDebugSphere(MeshComp->GetWorld(), CurrentLocation, Radius, 12, FColor::Red, false, 0.2f);
//		DrawDebugLine(MeshComp->GetWorld(), PreviousHandLocation, CurrentLocation, FColor::Green, false, 0.2f);
//#endif

	if (bHit)
	{
		for (const FHitResult& Hit : HitResults)
		{
			AActor* HitActor = Hit.GetActor();
			if (HitActor && HitActor != Character)
			{
				ETeamAttitude::Type Attitude = Character->GetTeamAttitudeTowards(*HitActor);
				if (Attitude == ETeamAttitude::Hostile)
				{
					const auto CharacterController = Character->GetController();
					if (CharacterController)
					{
						UE_LOG(LogTemp, Display, TEXT("Melee Swing: %s"), *HitActor->GetName());
						HitActor->TakeDamage(MeleeSwingDamage, FDamageEvent{}, CharacterController, Character);
						bIsDamageDone = true;
						break;
					}
				}
			}
		}
	}

	PreviousHandLocation = CurrentLocation;
}

void UGP_MeleeSwingAnimNotifyState::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	PreviousHandLocation = FVector::ZeroVector;
	bIsDamageDone = false;
}
