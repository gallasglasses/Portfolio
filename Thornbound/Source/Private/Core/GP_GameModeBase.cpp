// Fill out your copyright notice in the Description page of Project Settings.


#include "Core/GP_GameModeBase.h"

DEFINE_LOG_CATEGORY_STATIC(GP_GameModeBaseLog, All, All);

void AGP_GameModeBase::SetGameplayState(EGameplayState State)
{
	if (GameplayState == State) return;

	EGameplayState PreviousGameplayState = GameplayState;
	GameplayState = State;

	UE_LOG(GP_GameModeBaseLog, Display, TEXT("EGameplayState changed from %s to %s"),
		*StaticEnum<EGameplayState>()->GetNameStringByValue((int64)PreviousGameplayState),
		*StaticEnum<EGameplayState>()->GetNameStringByValue((int64)GameplayState)
	);

	OnGameplayStateChanged.Broadcast(PreviousGameplayState, GameplayState);
}

void AGP_GameModeBase::HandleDeathState()
{
	SetGameplayState(EGameplayState::DeathState);
}

void AGP_GameModeBase::StartQTEState()
{
	SetGameplayState(EGameplayState::QTEState);
}

void AGP_GameModeBase::FinishQTEState()
{
	SetGameplayState(EGameplayState::PlayState);
}

void AGP_GameModeBase::HandlePauseState(APlayerController* PC)
{
	SetPause(PC);
}

void AGP_GameModeBase::HandleUnPauseState()
{
	ClearPause();
}

void AGP_GameModeBase::StartPlay()
{
	Super::StartPlay();

	SetGameplayState(EGameplayState::PlayState);
}

bool AGP_GameModeBase::SetPause(APlayerController* PC, FCanUnpause CanUnpauseDelegate)
{
	const auto PauseSet = Super::SetPause(PC, CanUnpauseDelegate);
	if (PauseSet)
	{
		SetGameplayState(EGameplayState::PauseState);
	}

	return PauseSet;
}

bool AGP_GameModeBase::ClearPause()
{
	const auto PauseCleared = Super::ClearPause();
	if (PauseCleared)
	{
		SetGameplayState(EGameplayState::PlayState);
	}

	return PauseCleared;
}
