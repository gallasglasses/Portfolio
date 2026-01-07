// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GP_GameModeBase.generated.h"

UENUM(BlueprintType)
enum class EGameplayState : uint8
{
	InitState,
	PlayState,
	QTEState,
	PauseState,
	DeathState
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnGameplayStateChangedSignature, EGameplayState, PreviosGameplayState, EGameplayState, NextGameplayState);

UCLASS()
class GP3_TEAM10_API AGP_GameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, Category = "GameplayState")
	FOnGameplayStateChangedSignature OnGameplayStateChanged;

	UFUNCTION(BlueprintCallable)
	FORCEINLINE EGameplayState GetGameplayState() const { return GameplayState; }

	UFUNCTION(BlueprintCallable)
	void HandleDeathState();

	UFUNCTION(BlueprintCallable)
	void StartQTEState();

	UFUNCTION(BlueprintCallable)
	void FinishQTEState();

	UFUNCTION(BlueprintCallable)
	void HandlePauseState(APlayerController* PC);

	UFUNCTION(BlueprintCallable)
	void HandleUnPauseState();

	virtual void StartPlay() override;

	virtual bool SetPause(APlayerController* PC, FCanUnpause CanUnpauseDelegate = FCanUnpause()) override;

	virtual bool ClearPause() override;

private:

	UPROPERTY()
	EGameplayState GameplayState = EGameplayState::InitState;

	UFUNCTION(BlueprintCallable)
	void SetGameplayState(EGameplayState State);
};
