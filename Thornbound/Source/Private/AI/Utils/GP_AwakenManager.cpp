// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Utils/GP_AwakenManager.h"
#include "AI/GP_AICharacter.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "Interfaces/GP_FlowerTotemInterface.h"
#include "Components/GP_FlowerTotemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Components/BoxComponent.h"
#include "Engine/OverlapResult.h"
#include "Components/GP_HealthComponent.h"

DEFINE_LOG_CATEGORY_STATIC(GP_AwakenManagerLog, All, All);

AGP_AwakenManager::AGP_AwakenManager()
{
	PrimaryActorTick.bCanEverTick = false;

	USceneComponent* Root = CreateDefaultSubobject<USceneComponent>("RootComponent");
	SetRootComponent(Root);

	DetectionBox = CreateDefaultSubobject<UBoxComponent>("DetectionBox");
	DetectionBox->SetupAttachment(RootComponent);

}

void AGP_AwakenManager::BeginPlay()
{
	Super::BeginPlay();

	if (FlowerTotem)
	{
		UE_LOG(GP_AwakenManagerLog, Log, TEXT("FlowerTotem: %s"), *FlowerTotem->GetName());
		UGP_FlowerTotemComponent* FlowerComponent = FlowerTotem->GetComponentByClass<UGP_FlowerTotemComponent>();
		if (FlowerComponent && FlowerComponent->Implements<UGP_FlowerTotemInterface>())
		{
			UE_LOG(GP_AwakenManagerLog, Log, TEXT("FlowerTotem implements IGP_FlowerTotemInterface"));

			IGP_FlowerTotemInterface* FlowerTotemInterface = Cast<IGP_FlowerTotemInterface>(FlowerComponent);
			if (FlowerTotemInterface)
			{
				UE_LOG(GP_AwakenManagerLog, Log, TEXT("Subscribing to FlowerTotemPickedUp delegate"));

				FlowerTotemInterface->GetFlowerTotemPickedUpDelegate().AddDynamic(this, &AGP_AwakenManager::StartAwakening);
			}
		}
	}

	size_t i = 0;
	while (EnemiesToAwake.Num() > 0 && i < EnemiesToAwake.Num())
	{
		if (!IsValid(EnemiesToAwake[i]))
		{
			EnemiesToAwake.RemoveAt(i);
		}
		else
		{
			i++;
		}
	}

	if (EnemiesToAwake.Num() > 0)
	{
		UE_LOG(GP_AwakenManagerLog, Log, TEXT("Freezing %d enemies"), EnemiesToAwake.Num());

		for (i = 0; i < EnemiesToAwake.Num(); i++)
		{
			AGP_AICharacter* Enemy = EnemiesToAwake[i];
			if (Enemy)
			{
				UE_LOG(GP_AwakenManagerLog, Log, TEXT("Freezing Enemy: %s"), *Enemy->GetName());

				UGP_HealthComponent* EnemyHealth = Enemy->GetComponentByClass<UGP_HealthComponent>();
				if (EnemyHealth)
				{
					EnemyHealth->SetIsImmortal(true);
				}

				AAIController* AIController = Cast<AAIController>(Enemy->GetController());
				if (AIController && AIController->BrainComponent)
				{
					AIController->BrainComponent->PauseLogic("Freeze Enemy");
				}
				else
				{
					UE_LOG(GP_AwakenManagerLog, Warning, TEXT("Enemy %s has no valid AIController or BrainComponent"), *Enemy->GetName());
				}
			}
		}
	}
}

void AGP_AwakenManager::StartAwakening()
{
	UE_LOG(GP_AwakenManagerLog, Log, TEXT("StartAwakening called"));

	CurrentIndex = 0;
	AwakeNextEnemy();
}

#if WITH_EDITOR

void AGP_AwakenManager::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	const FName PropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : NAME_None;
	if (PropertyName == GET_MEMBER_NAME_CHECKED(AGP_AwakenManager, DetectionBox))
	{
		UpdateEnemiesInBox();
	}
}

void AGP_AwakenManager::UpdateEnemiesInBox()
{
	EnemiesToAwake.Empty();

	TArray<FOverlapResult> OverlappingResults;
	FVector BoxLocation = DetectionBox->GetComponentLocation();
	FVector BoxExtent = DetectionBox->GetScaledBoxExtent();
	FCollisionShape BoxShape = FCollisionShape::MakeBox(BoxExtent);

	bool bHasOverlap = GetWorld()->OverlapMultiByObjectType(OverlappingResults, BoxLocation, FQuat::Identity, FCollisionObjectQueryParams(ECollisionChannel::ECC_Pawn), BoxShape);

	if (bHasOverlap)
	{
		for (auto& Result : OverlappingResults)
		{
			AActor* Actor = Result.GetActor();
			if (AwakeningZombieClass && Actor && Actor->IsA(AwakeningZombieClass))
			{
				if (!EnemiesToAwake.Contains(Actor))
				{
					EnemiesToAwake.AddUnique(Cast<AGP_AICharacter>(Actor));
				}
			}
		}
	}

	UE_LOG(GP_AwakenManagerLog, Log, TEXT("Found %d unique enemies inside box"), EnemiesToAwake.Num());
}

void AGP_AwakenManager::SelectEnemiesInEditor()
{
	if (GEditor)
	{
		GEditor->SelectNone(false, true, false);

		for (AGP_AICharacter* Enemy : EnemiesToAwake)
		{
			if (Enemy)
			{
				GEditor->SelectActor(Enemy, true, true, true);
			}
		}
	}
}

void AGP_AwakenManager::DeleteUnvalidEnemies()
{
	size_t i = 0;
	while (EnemiesToAwake.Num() > 0 && i < EnemiesToAwake.Num())
	{
		if (!IsValid(EnemiesToAwake[i]))
		{
			EnemiesToAwake.RemoveAt(i);
		}
		else
		{
			i++;
		}
	}
}

#endif

void AGP_AwakenManager::AwakeNextEnemy()
{
	if (CurrentIndex >= EnemiesToAwake.Num())
	{
		UE_LOG(GP_AwakenManagerLog, Log, TEXT("All enemies awakened"));
		GetWorld()->GetTimerManager().ClearTimer(AwakeTimer);
		return;
	}

	AGP_AICharacter* Enemy = EnemiesToAwake[CurrentIndex];
	if (Enemy)
	{
		UE_LOG(GP_AwakenManagerLog, Log, TEXT("Awakening Enemy: %s"), *Enemy->GetName());

		AAIController* AIController = Cast<AAIController>(Enemy->GetController());
		if (AIController && AIController->BrainComponent)
		{
			UE_LOG(GP_AwakenManagerLog, Log, TEXT("Resuming Brain Logic for %s"), *Enemy->GetName());

			AIController->BrainComponent->ResumeLogic("Awake Enemy");
		}
		// set FBlackboardKeySelector TargetActorKey

		APawn* PlayerPawn = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		if (PlayerPawn)
		{
			UBlackboardComponent* BlackboardComp = AIController->GetBlackboardComponent();
			if (BlackboardComp)
			{
				BlackboardComp->SetValueAsBool(IsFlowerTotemEventKeyName, true);
				BlackboardComp->SetValueAsObject(TargetActorKeyName, PlayerPawn);
				AIController->SetFocus(PlayerPawn);

				UE_LOG(GP_AwakenManagerLog, Log, TEXT("Set TargetActorKey to Player %s for %s"), *PlayerPawn->GetName(), *Enemy->GetName());
			}
			else
			{
				UE_LOG(GP_AwakenManagerLog, Warning, TEXT("BlackboardComponent not found for %s"), *Enemy->GetName());
			}
		}
	}

	CurrentIndex++;
	GetWorld()->GetTimerManager().SetTimer(AwakeTimer, this, &AGP_AwakenManager::AwakeNextEnemy, DelayBetweenAwake, true);
}