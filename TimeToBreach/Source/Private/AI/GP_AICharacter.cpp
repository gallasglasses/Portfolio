// Galla

#include "AI/GP_AICharacter.h"
#include "AI/GP_AIController.h"
#include "AI/Components/GP_AICharacterMovementComponent.h"
#include "AI/Utils/GP_PatrolRouteActor.h"
#include "AI/Utils/GP_AttackTokenSubsystem.h"
#include "Components/CapsuleComponent.h"
#include "Navigation/CrowdManager.h"

#include "../../GameModes/GameplayGamemode.h"
#include "BehaviorTree/BlackboardComponent.h"

DEFINE_LOG_CATEGORY_STATIC(GP_AICharacterLog, All, All);

AGP_AICharacter:: AGP_AICharacter(const FObjectInitializer& ObjInit):
	Super(ObjInit.SetDefaultSubobjectClass<UGP_AICharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	PrimaryActorTick.bCanEverTick = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	AIControllerClass = AGP_AIController::StaticClass();

	bUseControllerRotationYaw = false;
}

void AGP_AICharacter::BeginPlay()
{
	Super::BeginPlay();

	/*auto CrowdManager = UCrowdManager::GetCurrent(this);
	if (CrowdManager)
	{
		CrowdManager->RegisterAgent(this);
	}*/

	if (!GetWorld()) return;

	AGameplayGamemode* GameplayGameMode = Cast<AGameplayGamemode>(GetWorld()->GetAuthGameMode());
	if (!GameplayGameMode) return;

	GameplayGameMode->OnStartGlobalNormalTime.AddDynamic(this, &AGP_AICharacter::OnStartGlobalNormalTime);
	GameplayGameMode->OnStopGlobalNormalTime.AddDynamic(this, &AGP_AICharacter::OnStopGlobalNormalTime);

	RemainedStunnedCooldownTime = StunnedTime;
}

void AGP_AICharacter::BeginDestroy()
{
	Super::BeginDestroy();

	/*auto CrowdManager = UCrowdManager::GetCurrent(this);
	if (CrowdManager)
	{
		CrowdManager->UnregisterAgent(this);
	}*/
}

void AGP_AICharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (AIController)
	{
		if (AIController->GetAIState() == EAIState::Stunned)
		{
			//UE_LOG(GP_AICharacterLog, Display, TEXT("GetAIState() == EAIState::Stunned"));
			if (RemainedStunnedCooldownTime <= 0.f)
			{
				AIController->ResetStunnedState();
				RemainedStunnedCooldownTime = StunnedTime;
			}
			else
			{
				RemainedStunnedCooldownTime -= DeltaTime * TimeDilation;
				AIController->SetStunnedRemainedTime(RemainedStunnedCooldownTime);
			}
		}
	}
	else
	{
		AIController = Cast<AGP_AIController>(Controller);
	}
}

void AGP_AICharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AGP_AICharacter::HandleDeath()
{
	if (!AIController) return;

	UGP_AttackTokenSubsystem* AttackTokenSubsystem = GetWorld()->GetSubsystem<UGP_AttackTokenSubsystem>();
	if (AttackTokenSubsystem)
	{
		AttackTokenSubsystem->ReleaseToken(AIController, 1);
	}
	AIController->HandleDeath();
	GetCharacterMovement()->DisableMovement();
	GetCapsuleComponent()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore); //ECC_GameTraceChannel1
	GetMesh()->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
}

//FVector AGP_AICharacter::GetCrowdAgentLocation() const
//{
//	return GetActorLocation();
//}
//
//FVector AGP_AICharacter::GetCrowdAgentVelocity() const
//{
//	return GetCharacterMovement()->GetVelocityForRVOConsideration();
//}
//
//void AGP_AICharacter::GetCrowdAgentCollisions(float& CylinderRadius, float& CylinderHalfHeight) const
//{
//	CylinderRadius = GetCapsuleComponent()->GetScaledCapsuleRadius();
//	CylinderHalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
//}
//
//float AGP_AICharacter::GetCrowdAgentMaxSpeed() const
//{
//	return GetCharacterMovement()->GetMaxSpeed();
//}

void AGP_AICharacter::OnStartGlobalNormalTime()
{
	TimeDilation = 1.0f;
}

void AGP_AICharacter::OnStopGlobalNormalTime(float Time)
{
	TimeDilation = Time;
}