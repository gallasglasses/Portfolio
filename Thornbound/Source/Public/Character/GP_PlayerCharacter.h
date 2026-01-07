// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "Interfaces/GP_QTETargetInterface.h"
#include "GP_PlayerCharacter.generated.h"

class UGP_QTEComponent;

UCLASS()
class GP3_TEAM10_API AGP_PlayerCharacter : public ACharacter, public IGenericTeamAgentInterface, public IGP_QTETargetInterface
{
	GENERATED_BODY()

public:

	AGP_PlayerCharacter();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Player|Components")
	UGP_QTEComponent* QTEComponent;

	UPROPERTY(VisibleAnywhere, Category = "Generic Team Id")
	FGenericTeamId TeamID = 3;

public:

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Generic Team Id")
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override { TeamID = NewTeamID; };

	UFUNCTION(BlueprintCallable, Category = "Generic Team Id")
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamID; }

	virtual void StartQTE_Implementation(AActor* InstigatorActor) override;

};
