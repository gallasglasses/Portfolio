// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/GP_QTEComponent.h"
#include "Core/GP_GameModeBase.h"
#include "AI/GP_AICharacter.h"
#include "Interfaces/GP_QTETargetInterface.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/GP_HealthComponent.h"

DEFINE_LOG_CATEGORY_STATIC(GP_QTEComponentLog, All, All);

UGP_QTEComponent::UGP_QTEComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

}

void UGP_QTEComponent::StartQTE(AActor* Instigator)
{
	if(!GetWorld())
	{
		UE_LOG(GP_QTEComponentLog, Warning, TEXT("World is not valid"));
		return;
	}
	if (bQTEActive)
	{
		UE_LOG(GP_QTEComponentLog, Warning, TEXT("StartQTE called, but QTE is not active"));
		return;
	}
	if(!Instigator)
	{
		UE_LOG(GP_QTEComponentLog, Warning, TEXT("Instigator is not valid"));
		return;
	}

	UGP_HealthComponent* HealthComponent = GetOwner()->GetComponentByClass<UGP_HealthComponent>();
	if (HealthComponent)
	{
		HealthComponent->OnDeath.AddUniqueDynamic(this, &UGP_QTEComponent::FailQTE);
	}
	QTEProgress = 0.0f;
	QTEElapsedTime = 0.0f;
	QTEInstigator = Instigator;
	bQTEActive = true;

	GetWorld()->GetTimerManager().SetTimer(QTETickHandle, this, &UGP_QTEComponent::UpdateQTE, QTETickTime, true);
	GetWorld()->GetTimerManager().SetTimer(QTEFailHandle, this, &UGP_QTEComponent::FailQTE, QTEFillTime, false);
	UE_LOG(GP_QTEComponentLog, Display, TEXT("%s started QTE: %s"), *GetName(), bQTEActive ? TEXT("TRUE") : TEXT("FALSE"));
}

void UGP_QTEComponent::OnQTEButtonPressed()
{
	if (!bQTEActive)
	{
		UE_LOG(GP_QTEComponentLog, Display, TEXT("OnQTEButtonPressed:: bQTEActive: %s"), bQTEActive ? TEXT("TRUE") : TEXT("FALSE"));
		return;
	}

	QTEProgress += QTEDrainPerPress;
	QTEProgress = FMath::Clamp(QTEProgress, 0.0f, QTEThreshold);

	UE_LOG(GP_QTEComponentLog, Display, TEXT("QTE Button pressed! Progress = %f / %f"), QTEProgress, QTEThreshold);

	if (QTEProgress >= QTEThreshold)
	{
		bQTEActive = false;
		GetWorld()->GetTimerManager().ClearTimer(QTETickHandle);
		GetWorld()->GetTimerManager().ClearTimer(QTEFailHandle);

		UE_LOG(GP_QTEComponentLog, Display, TEXT("QTE Success! QTE completed"));

		if (!QTEInstigator.IsValid()) return;

		AActor* QTEInstigatorActor = QTEInstigator.Get();

		if (AGP_AICharacter* Enemy = Cast<AGP_AICharacter>(QTEInstigatorActor))
		{
			Enemy->OnQTEFinished.Broadcast(true);
		}

		if (!GetWorld() || !GetWorld()->GetAuthGameMode()) return;
		const auto GameMode = Cast<AGP_GameModeBase>(GetWorld()->GetAuthGameMode());
		if (GameMode)
		{
			GameMode->FinishQTEState();
		}

		ACharacter* Player = Cast<ACharacter>(GetOwner());
		if (Player)
		{
			if (Player->GetCharacterMovement())
			{
				Player->GetCharacterMovement()->SetMovementMode(EMovementMode::MOVE_Walking);
			}
			if (Player->GetClass()->ImplementsInterface(UGP_QTETargetInterface::StaticClass()))
			{
				IGP_QTETargetInterface::Execute_FinishQTE(Player);
			}
		}
	}
}

void UGP_QTEComponent::CancelQTE()
{
	bQTEActive = false;
	GetWorld()->GetTimerManager().ClearTimer(QTETickHandle);
	GetWorld()->GetTimerManager().ClearTimer(QTEFailHandle);
}

void UGP_QTEComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UGP_QTEComponent::UpdateQTE()
{
	if (!bQTEActive) return;

	QTEElapsedTime += QTETickTime;
	float TimeLeft = FMath::Clamp(QTEFillTime - QTEElapsedTime, 0.0f, QTEFillTime);

	OnQTEProgressUpdate.Broadcast(QTEProgress / QTEThreshold, TimeLeft / QTEFillTime);
}

void UGP_QTEComponent::FailQTE()
{
	bQTEActive = false;
	GetWorld()->GetTimerManager().ClearTimer(QTETickHandle);
	GetWorld()->GetTimerManager().ClearTimer(QTEFailHandle);

	if (!QTEInstigator.IsValid()) return;

	AActor* QTEInstigatorActor = QTEInstigator.Get();

	if (AGP_AICharacter* Enemy = Cast<AGP_AICharacter>(QTEInstigatorActor))
	{
		Enemy->OnQTEFinished.Broadcast(false);
	}
}

