// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Interfaces/GP_FlowerTotemInterface.h"
#include "GP_FlowerTotemComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GP3_TEAM10_API UGP_FlowerTotemComponent : public UActorComponent, public IGP_FlowerTotemInterface
{
	GENERATED_BODY()

public:

	UGP_FlowerTotemComponent();

protected:

	virtual void BeginPlay() override;

public:

	UPROPERTY(BlueprintAssignable, Category = "FlowerTotem")
	FOnFlowerTotemPickedUpSignature OnFlowerTotemPickedUp;

	UFUNCTION(BlueprintCallable, Category = "FlowerTotem")
	virtual void OnPickedUp_Implementation() override;

	virtual FOnFlowerTotemPickedUpSignature& GetFlowerTotemPickedUpDelegate() override;
};
