// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GP_AwakenManager.generated.h"

class AGP_AICharacter;
class UBoxComponent;

UCLASS()
class GP3_TEAM10_API AGP_AwakenManager : public AActor
{
	GENERATED_BODY()
	
public:

	AGP_AwakenManager();

protected:

	virtual void BeginPlay() override;


	UPROPERTY(EditDefaultsOnly, Category = "GP_AwakenManager")
	TSubclassOf<AGP_AICharacter> AwakeningZombieClass;

	UPROPERTY(EditAnywhere, Category = "GP_AwakenManager")
	TArray<AGP_AICharacter*> EnemiesToAwake;

	UPROPERTY(EditAnywhere, Category = "GP_AwakenManager")
	UBoxComponent* DetectionBox;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GP_AwakenManager")
	FName TargetActorKeyName = "TargetActor";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GP_AwakenManager")
	FName IsFlowerTotemEventKeyName = "IsFlowerTotemEvent";

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GP_AwakenManager")
	AActor* FlowerTotem;

	UPROPERTY(EditAnywhere, Category = "GP_AwakenManager", meta = (ClampMin = "0.01", ClampMax = "600.0"))
	float DelayBetweenAwake = 1.0f;

	UFUNCTION()
	void StartAwakening();

#if WITH_EDITOR

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
	UFUNCTION(CallInEditor, Category = "GP_AwakenManager")
	void UpdateEnemiesInBox();

	UFUNCTION(CallInEditor, Category = "GP_AwakenManager")
	void SelectEnemiesInEditor();

	UFUNCTION(CallInEditor, Category = "GP_AwakenManager")
	void DeleteUnvalidEnemies();

#endif

private:

	size_t CurrentIndex = 0;

	FTimerHandle AwakeTimer;

	void AwakeNextEnemy();

};
