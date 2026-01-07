// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Tasks/GP_CrawlerAttackTask.h"
#include "AIController.h"
#include "AI/GP_AICharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "GameFramework/Character.h"
#include "Interfaces/GP_QTETargetInterface.h"
#include "Engine/DamageEvents.h"
#include "Components/GP_HealthComponent.h"
#include "Core/GP_AttackTokenSubsystem.h"

DEFINE_LOG_CATEGORY_STATIC(GP_CrawlerAttackTaskLog, All, All);

UGP_CrawlerAttackTask::UGP_CrawlerAttackTask()
{
    NodeName = "Crawler Attack";
    bNotifyTick = false;
}

EBTNodeResult::Type UGP_CrawlerAttackTask::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    const auto Controller = OwnerComp.GetAIOwner();
    const auto Blackboard = OwnerComp.GetBlackboardComponent();

    if (!Controller || !Blackboard)
    {
        UE_LOG(GP_CrawlerAttackTaskLog, Display, TEXT("Controller Blackboard Not Valid"));
        return EBTNodeResult::Failed;
    }

    const auto Crawler = Cast<AGP_AICharacter>(Controller->GetPawn());
    if (!Crawler)
    {
        UE_LOG(GP_CrawlerAttackTaskLog, Display, TEXT("Crawler Not Valid"));
        return EBTNodeResult::Failed;
    }

    UBehaviorTreeComponent* CapturedComp = &OwnerComp;
    CapturedComponent = CapturedComp;
    AAIController* CapturedController = Controller;

    AActor* Player = Cast<AActor>(Blackboard->GetValueAsObject(TargetActorKey.SelectedKeyName));

    if (Player && Player->GetClass()->ImplementsInterface(UGP_QTETargetInterface::StaticClass()))
    {
        if (!Crawler->OnQTEFinished.IsAlreadyBound(this, &UGP_CrawlerAttackTask::OnQTEComplete))
        {
            Crawler->OnQTEFinished.AddDynamic(this, &UGP_CrawlerAttackTask::OnQTEComplete);
        }

        UGP_AttackTokenSubsystem* AttackTokenSubsystem = GetWorld()->GetSubsystem<UGP_AttackTokenSubsystem>();
        if (!AttackTokenSubsystem)
        {
            UE_LOG(GP_CrawlerAttackTaskLog, Error, TEXT("No AttackTokenSubsystem in world"));
            return EBTNodeResult::Failed;
        }

        if (AttackTokenSubsystem->RequestToken(Controller, 1))
        {
            Blackboard->SetValueAsBool(IsAttackingKey.SelectedKeyName, true);
            Crawler->StartQTE(Player);

            UE_LOG(GP_CrawlerAttackTaskLog, Display, TEXT("Player : %s | Crawler : %s"), *Player->GetName(), *Crawler->GetName());
            IGP_QTETargetInterface::Execute_StartQTE(Player, Crawler);

            Controller->BrainComponent->PauseLogic("QTE In Progress");

            if (Crawler->OnFinishedAttack.IsBound())
            {
                Crawler->OnFinishedAttack.Clear();
            }

            Crawler->OnFinishedAttack.AddLambda(
                [this, CapturedComp, CapturedController]()
                {
                    if (!bQTECompleted) return;
                    if (!CapturedComp || !CapturedController) return;

                    const auto MemoryBlackboard = CapturedComp->GetBlackboardComponent();
                    if (!MemoryBlackboard)
                    {
                        FinishLatentTask(*CapturedComp, EBTNodeResult::Failed);
                        return;
                    }

                    UGP_AttackTokenSubsystem* AttackTokenSubsystem = GetWorld()->GetSubsystem<UGP_AttackTokenSubsystem>();
                    if (!AttackTokenSubsystem)
                    {
                        FinishLatentTask(*CapturedComp, EBTNodeResult::Failed);
                        return;
                    }

                    AttackTokenSubsystem->ReleaseToken(CapturedController, 1);

                    if (bQTECompleted)
                    {
                        AGP_AICharacter* Owner = Cast<AGP_AICharacter>(CapturedController->GetPawn());
                        if (!Owner)
                        {
                            FinishLatentTask(*CapturedComp, EBTNodeResult::Failed);
                            return;
                        }
                        const UGP_HealthComponent* HealthComponent = Owner->GetComponentByClass<UGP_HealthComponent>();
                        if (HealthComponent)
                        {
                            if (!HealthComponent->IsDead())
                            {
                                Owner->SetCurrentAnimState(EAIAnimState::Idle);
                            }
                            else
                            {
                                Owner->SetCurrentAnimState(EAIAnimState::Death);
                                FinishLatentTask(*CapturedComp, EBTNodeResult::Failed);
                            }
                        }
                        else
                        {
                            FinishLatentTask(*CapturedComp, EBTNodeResult::Failed);
                        }

                        CapturedController->BrainComponent->ResumeLogic("QTE Finished");
                        MemoryBlackboard->SetValueAsBool(IsAttackingKey.SelectedKeyName, false);
                        FinishLatentTask(*CapturedComp, EBTNodeResult::Succeeded);
                        bQTECompleted = false;
                    }
                    if (CapturedController->BrainComponent->IsPaused())
                    {
                        CapturedController->BrainComponent->ResumeLogic("QTE Finished");
                    }
                }
            );
        }
        else
        {
            UE_LOG(GP_CrawlerAttackTaskLog, Error, TEXT("Request Token is denied for %s"), *Crawler->GetName());
            return EBTNodeResult::Failed;
        }
        
    }
    else
    {
        UE_LOG(GP_CrawlerAttackTaskLog, Display, TEXT("Target actor is notvalid"));
        return EBTNodeResult::Failed;
    }

    return EBTNodeResult::InProgress;
}

void UGP_CrawlerAttackTask::TickTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{

}

void UGP_CrawlerAttackTask::OnQTEComplete(bool bSuccess)
{
    if (!CapturedComponent) return;

    const auto Controller = CapturedComponent->GetAIOwner();
    const auto Blackboard = CapturedComponent->GetBlackboardComponent();
    if (!Controller || !Blackboard) return;

    const auto Crawler = Cast<AGP_AICharacter>(Controller->GetPawn());
    if (!Crawler)
    {
        FinishLatentTask(*CapturedComponent, EBTNodeResult::Failed);
    }

    ACharacter* Player = Cast<ACharacter>(Blackboard->GetValueAsObject(TargetActorKey.SelectedKeyName));

    UE_LOG(GP_CrawlerAttackTaskLog, Display, TEXT("OnQTEComplete"));
    if (bSuccess)
    {
        if (Crawler && Player)
        {
            const UGP_HealthComponent* HealthComponent = Crawler->GetComponentByClass<UGP_HealthComponent>();
            if (HealthComponent)
            {
                if (!HealthComponent->IsDead())
                {
                    FVector PushBack = (Crawler->GetActorLocation() - Player->GetActorLocation()).GetSafeNormal();
                    Crawler->LaunchCharacter(PushBack * Crawler->GetPushingForce(), true, true);

                    Crawler->TakeDamage(Crawler->GetDealtDamageAfterQTE(), FDamageEvent{}, Player->GetController(), Player);
                }
            }
        }
    }
    else
    {
        // death to player
        if (Player)
        {
            Player->TakeDamage(MAX_FLT, FDamageEvent{}, Controller, Crawler);
        }
    }

    Blackboard->SetValueAsBool(IsAttackingKey.SelectedKeyName, false);
    if (Crawler)
    {
        if (Controller->BrainComponent->IsPaused())
        {
            Controller->BrainComponent->ResumeLogic("QTE Finished");
        }
        Crawler->FinishQTE();
        Crawler->OnQTEFinished.RemoveDynamic(this, &UGP_CrawlerAttackTask::OnQTEComplete);
        bQTECompleted = true;
    }
    else
    {
        FinishLatentTask(*CapturedComponent, EBTNodeResult::Failed);
    }
    UE_LOG(GP_CrawlerAttackTaskLog, Display, TEXT("OnQTEFinished %s"), bSuccess ? TEXT("TRUE") : TEXT("FALSE"));
}

EBTNodeResult::Type UGP_CrawlerAttackTask::AbortTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
    AAIController* Controller = OwnerComp.GetAIOwner();
    if (Controller)
    {
        if (auto* Owner = Cast<AGP_AICharacter>(Controller->GetPawn()))
        {
            Owner->OnFinishedAttack.RemoveAll(this);
            Owner->OnQTEFinished.RemoveAll(this);
            if (Controller->BrainComponent->IsPaused())
            {
                Controller->BrainComponent->ResumeLogic("QTE Finished");
            }
        }
    }
    return Super::AbortTask(OwnerComp, NodeMemory);
}
