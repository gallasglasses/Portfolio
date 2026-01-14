// Galla


#include "AI/Components/GP_AIPerceptionComponent.h"
#include "Perception/AISense_Sight.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Hearing.h"
#include "Perception/AISense_Prediction.h"
#include "AIController.h"
#include "AI/GP_AIController.h"
#include "AI/GP_AICharacter.h"

#include "NavigationSystem.h"
#include "NavigationPath.h"

#include "AI/Utils/GP_AISubsystem.h"


DEFINE_LOG_CATEGORY_STATIC(GP_AIPerceptionComponentLog, All, All);

void UGP_AIPerceptionComponent::BeginPlay()
{
	Super::BeginPlay();
	OnTargetPerceptionUpdated.AddDynamic(this, &UGP_AIPerceptionComponent::OnPerceptionTargetUpdated);
	OnTargetPerceptionForgotten.AddDynamic(this, &UGP_AIPerceptionComponent::OnPerceptionTargetForgotten);

	if (GetWorld()->GetSubsystem<UGP_AISubsystem>())
	{
		AISubsytem = GetWorld()->GetSubsystem<UGP_AISubsystem>();
	}

	const auto Controller = Cast<AGP_AIController>(GetOwner());
	if (!Controller) return;

	Controller->OnAICharacterDeath.AddDynamic(this, &UGP_AIPerceptionComponent::HandleDeath);
}

void UGP_AIPerceptionComponent::FindActiveSense(AActor* Actor)
{
	//UE_LOG(GP_AIPerceptionComponentLog, Display, TEXT("FindActiveSense"));
	const auto Controller = Cast<AGP_AIController>(GetOwner());
	if (!Controller || !Actor || Controller->GetAIState() == EAIState::Dead) return;

	FActorPerceptionBlueprintInfo PerceptionInfo;
	GetActorsPerception(Actor, PerceptionInfo);

	for (const auto Stimulus : PerceptionInfo.LastSensedStimuli)
	{
		//UE_LOG(GP_AIPerceptionComponentLog, Display, TEXT("Stimulus : PerceptionInfo.LastSensedStimuli"));
		if (!Stimulus.IsExpired() && Stimulus.WasSuccessfullySensed())
		{
			if (Stimulus.Type == UAISense::GetSenseID(UAISense_Sight::StaticClass()))
			{
				//UE_LOG(GP_AIPerceptionComponentLog, Display, TEXT("FindActiveSense UAISense_Sight"));
				
				Controller->HandleSensedSight(Actor);
				return;
			}
			else if (Stimulus.Type == UAISense::GetSenseID(UAISense_Hearing::StaticClass()))
			{
				//UE_LOG(GP_AIPerceptionComponentLog, Display, TEXT("FindActiveSense UAISense_Hearing"));
				
				Controller->HandleSensedSound(Stimulus.StimulusLocation, Actor);
				return;
			}
			else if (Stimulus.Type == UAISense::GetSenseID(UAISense_Damage::StaticClass()))
			{
				//UE_LOG(GP_AIPerceptionComponentLog, Display, TEXT("FindActiveSense UAISense_Damage"));
				
				Controller->HandleSensedDamage(Stimulus.StimulusLocation, Actor);
				return;
			}
			else if (Stimulus.Type == UAISense::GetSenseID(UAISense_Prediction::StaticClass()))
			{
				//UE_LOG(GP_AIPerceptionComponentLog, Display, TEXT("FindActiveSense UAISense_Prediction"));

				Controller->HandleSensedPrediction(Stimulus.StimulusLocation, Actor);
				return;
			}
		}
	}
}

void UGP_AIPerceptionComponent::OnPerceptionTargetUpdated(AActor* Actor, FAIStimulus Stimulus)
{
	// check if its player-enemy

	const auto Controller = Cast<AGP_AIController>(GetOwner());
	if (!Controller || !Actor || Controller->GetAIState() == EAIState::Dead) return;

	const bool bSensed = Stimulus.WasSuccessfullySensed();

	if (bSensed)
	{
		AISubsytem->SetIsCharacterForgotten(Controller, false);
		if (Stimulus.Type == UAISense::GetSenseID(UAISense_Sight::StaticClass()))
		{
			StopPredictionLoop();
			Controller->HandleSensedSight(Actor);
			return;
		}
		else if (Stimulus.Type == UAISense::GetSenseID(UAISense_Hearing::StaticClass()))
		{
			StopPredictionLoop();
			Controller->HandleSensedSound(Stimulus.StimulusLocation, Actor);
			return;
		}
		else if (Stimulus.Type == UAISense::GetSenseID(UAISense_Damage::StaticClass()))
		{
			//UE_LOG(GP_AIPerceptionComponentLog, Display, TEXT("OnPerceptionTargetUpdated UAISense_Damage"));
			StopPredictionLoop();
			Controller->HandleSensedDamage(Stimulus.StimulusLocation, Actor);
			return;
		}
		else if (Stimulus.Type == UAISense::GetSenseID(UAISense_Prediction::StaticClass()))
		{
			Controller->HandleSensedPrediction(Stimulus.StimulusLocation, Actor);
			return;
		}
	}

	if (!bSensed && Stimulus.Type == UAISense::GetSenseID(UAISense_Sight::StaticClass()))
	{
		Controller->ClearTarget();

		const FVector LastLoc = Stimulus.StimulusLocation;
		Controller->HandleSensedPrediction(LastLoc, Actor);

		StartPredictionLoop(Actor);
		return;
	}
}

void UGP_AIPerceptionComponent::OnPerceptionTargetForgotten(AActor* Actor)
{
	//UE_LOG(GP_AIPerceptionComponentLog, Display, TEXT("Forgot a %s "), *Actor->GetName());
	const auto Controller = Cast<AGP_AIController>(GetOwner());
	if (!Controller || !Actor) return;

	if (!PredictedActor.IsValid())
	{
		StartPredictionLoop(Actor);
	}
}

void UGP_AIPerceptionComponent::StartPredictionLoop(AActor* LostActor)
{
	PredictedActor = LostActor;
	PredictionLoopStopAtTime = GetWorld()->GetTimeSeconds() + PredictionLoopMaxDuration;

	TickPredictionLoop();

	GetWorld()->GetTimerManager().SetTimer(PredictionLoopTimer, this, &UGP_AIPerceptionComponent::TickPredictionLoop, PredictionTickInterval, true);
}

void UGP_AIPerceptionComponent::StopPredictionLoop()
{
	const auto Controller = Cast<AGP_AIController>(GetOwner());
	if (!Controller) return;

	if (PredictionLoopTimer.IsValid())
	{
		GetWorld()->GetTimerManager().ClearTimer(PredictionLoopTimer);
	}

	if (PredictedActor.IsValid())
	{
		ForgetActor(PredictedActor.Get());

		AISubsytem->SetIsCharacterForgotten(Controller, true);
		PredictedActor = nullptr;
	}

	//UE_LOG(GP_AIPerceptionComponentLog, Display, TEXT("StopPredictionLoop "));
}

void UGP_AIPerceptionComponent::TickPredictionLoop()
{
	AGP_AIController* Controller = Cast<AGP_AIController>(GetOwner());
	if (!Controller || !PredictedActor.IsValid())
		return;

	//UE_LOG(GP_AIPerceptionComponentLog, Display, TEXT("TickPredictionLoop "));
	const float Now = GetWorld()->GetTimeSeconds();
	if (Now >= PredictionLoopStopAtTime)
	{
		StopPredictionLoop();
		return;
	}

	UAISense_Prediction::RequestControllerPredictionEvent(Controller, PredictedActor.Get(), PredictionTime);
}

void UGP_AIPerceptionComponent::HandleDeath()
{
	AGP_AIController* Controller = Cast<AGP_AIController>(GetOwner());
	if (!Controller) return;

	StopPredictionLoop();
	ForgetAll();
	AISubsytem->UnregisterAIListener(Controller);
}