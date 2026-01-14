// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "GameFramework/Character.h"
#include "Team7CharacterBase.generated.h"

// showing just my part of code
UCLASS(Abstract)
class GP4_TEAM7_API ATeam7CharacterBase : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:
	//...

	UFUNCTION(BlueprintCallable, Category = "Generic Team Id")
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override { TeamID = NewTeamID; };

	UFUNCTION(BlueprintCallable, Category = "Generic Team Id")
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamID; }
	
protected:
	//...

	UPROPERTY(VisibleAnywhere, Category = "Generic Team Id")
	FGenericTeamId TeamID = 3;
};
