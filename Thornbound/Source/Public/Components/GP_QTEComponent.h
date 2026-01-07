// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GP_QTEComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQTECompleteSignature, bool, bSuccess);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnQTEProgressUpdateSignature, float, ProgressPercent, float, TimeLeftPercent);

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class GP3_TEAM10_API UGP_QTEComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UGP_QTEComponent();

	UFUNCTION(BlueprintCallable)
	void StartQTE(AActor* Instigator);

	UFUNCTION(BlueprintCallable)
	void OnQTEButtonPressed();

	UFUNCTION(BlueprintCallable)
	void CancelQTE();

	UPROPERTY(BlueprintAssignable)
	FOnQTEProgressUpdateSignature OnQTEProgressUpdate;

	UPROPERTY(BlueprintAssignable)
	FOnQTECompleteSignature OnQTEComplete;

protected:

	virtual void BeginPlay() override;

private:

	void UpdateQTE();

	UFUNCTION()
	void FailQTE();

	FTimerHandle QTETickHandle;
	FTimerHandle QTEFailHandle;

	//AActor* QTEInstigator;
	TWeakObjectPtr<AActor> QTEInstigator;

	bool bQTEActive = false;
	float QTEProgress = 0.0f;
	float QTEElapsedTime = 0.0f;

	UPROPERTY(EditDefaultsOnly, Category = "QTE")
	float QTETickTime = 0.05f;

	UPROPERTY(EditDefaultsOnly, Category = "QTE")
	float QTEFillTime = 3.0f;

	UPROPERTY(EditDefaultsOnly, Category = "QTE")
	float QTEThreshold = 1.0f;

	UPROPERTY(EditDefaultsOnly, Category = "QTE")
	float QTEDrainPerPress = 0.2f;
		
};
