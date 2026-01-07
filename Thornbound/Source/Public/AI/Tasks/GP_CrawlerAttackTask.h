// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GP_CrawlerAttackTask.generated.h"

class UBehaviorTreeComponent;

UCLASS()
class GP3_TEAM10_API UGP_CrawlerAttackTask : public UBTTaskNode
{
	GENERATED_BODY()

public:

    UGP_CrawlerAttackTask();

    virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;
    virtual void TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

protected:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    FBlackboardKeySelector TargetActorKey;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
    FBlackboardKeySelector IsAttackingKey;

    UFUNCTION()
    void OnQTEComplete(bool bSuccess);

    virtual EBTNodeResult::Type AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory);

private:

    UBehaviorTreeComponent* CapturedComponent = nullptr;

    bool bQTECompleted = false;
    bool bQTEStarted = false;
	
};
