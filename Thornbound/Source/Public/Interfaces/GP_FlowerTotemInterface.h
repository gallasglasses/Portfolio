// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "GP_FlowerTotemInterface.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnFlowerTotemPickedUpSignature);

UINTERFACE(MinimalAPI)
class UGP_FlowerTotemInterface : public UInterface
{
	GENERATED_BODY()
};

class GP3_TEAM10_API IGP_FlowerTotemInterface
{
	GENERATED_BODY()

public:

	virtual FOnFlowerTotemPickedUpSignature& GetFlowerTotemPickedUpDelegate() = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "FlowerTotem")
	void OnPickedUp();
};
