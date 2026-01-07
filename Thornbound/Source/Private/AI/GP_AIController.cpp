// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/GP_AIController.h"
#include "AI/GP_AICharacter.h"
#include "AI/Components/GP_AIPerceptionComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Perception/AISense_Damage.h"
#include "Components/GP_HealthComponent.h"

DEFINE_LOG_CATEGORY_STATIC(GP_AIController, All, All);

AGP_AIController::AGP_AIController()
{
	AIPerceptionComponent = CreateDefaultSubobject<UGP_AIPerceptionComponent>("AIPerceptionComponent");
	SetPerceptionComponent(*AIPerceptionComponent);

	SetGenericTeamId(FGenericTeamId(0));
}

ETeamAttitude::Type AGP_AIController::GetTeamAttitudeTowards(const AActor& Other) const
{
	ETeamAttitude::Type Attitude;
	const IGenericTeamAgentInterface* OtherTeamAgent = Cast<const IGenericTeamAgentInterface>(&Other);
	if (OtherTeamAgent)
	{
		Attitude = FGenericTeamId::GetAttitude(GetGenericTeamId(), OtherTeamAgent->GetGenericTeamId());
	}
	else
	{
		Attitude = ETeamAttitude::Neutral;
	}
	return Attitude;
}

void AGP_AIController::SetGenericTeamId(const FGenericTeamId& NewTeamID)
{
	Super::SetGenericTeamId(NewTeamID);

	IGenericTeamAgentInterface* ControlledAgent = Cast<IGenericTeamAgentInterface>(GetPawn());
	if (ControlledAgent)
	{
		ControlledAgent->SetGenericTeamId(NewTeamID);
	}
}

FGenericTeamId AGP_AIController::GetGenericTeamId() const
{
	const IGenericTeamAgentInterface* ControlledAgent = Cast<const IGenericTeamAgentInterface>(GetPawn());
	if (ControlledAgent)
	{
		return ControlledAgent->GetGenericTeamId();
	}

	return FGenericTeamId();
}

void AGP_AIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	const AGP_AICharacter* AICharacter = Cast<AGP_AICharacter>(InPawn);
	if (!IsValid(AICharacter))
	{
		UE_LOG(GP_AIController, Warning, TEXT("OnPossess: InPawn is not AGP_AICharacter"));
		return;
	}

	if (AICharacter->BehaviorTree)
	{
		RunBehaviorTree(AICharacter->BehaviorTree);
		UE_LOG(GP_AIController, Display, TEXT("OnPossess: RunBehaviorTree"));
	}

	const auto HealthComponent = AICharacter->GetComponentByClass<UGP_HealthComponent>();
	if (HealthComponent)
	{
		HealthComponent->OnDamage.AddDynamic(this, &AGP_AIController::HandleDamage);
	}
}

void AGP_AIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

AActor* AGP_AIController::GetFocusOnActor() const
{
	if (!GetBlackboardComponent()) return nullptr;

	return Cast<AActor>(GetBlackboardComponent()->GetValueAsObject(FocusOnKeyName));
}

void AGP_AIController::HandleDamage(float Damage, const UDamageType* DamageType, AController* InstigatedBy)
{
	if (!InstigatedBy)
	{
		UE_LOG(GP_AIController, Warning, TEXT("InstigatedBy is NULL"));
		return;
	}
	if (!InstigatedBy->GetPawn())
	{
		UE_LOG(GP_AIController, Warning, TEXT("InstigatedBy->GetPawn() is NULL"));
		return;
	}
	if (!GetPawn())
	{
		UE_LOG(GP_AIController, Warning, TEXT("GetPawn() is NULL"));
		return;
	}
	UE_LOG(GP_AIController, Display, TEXT("HandleDamage: Reporting damage from %s to %s"), *InstigatedBy->GetPawn()->GetName(), *GetPawn()->GetName());
	UAISense_Damage::ReportDamageEvent(GetWorld(), GetPawn(), InstigatedBy->GetPawn(), Damage, InstigatedBy->GetPawn()->GetActorLocation(), GetPawn()->GetActorLocation());
}

void AGP_AIController::SetReservedAttackTokens(int32 TokenAmount)
{
	if (TokenAmount < 0)
	{
		ReservedAttackTokens = 0;
	}
	else
	{
		ReservedAttackTokens = TokenAmount;
	}
}
