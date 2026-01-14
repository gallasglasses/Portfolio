// Galla

#pragma once

#include "CoreMinimal.h"
#include "GenericTeamAgentInterface.h"
#include "Navigation/CrowdAgentInterface.h"
#include "GameFramework/Character.h"
#include "GP_AICharacter.generated.h"

class UBehaviorTree;
class AGP_PatrolRouteActor;
class AGP_AIController;

UENUM(BlueprintType)
enum ESpeedState
{
	Idle,
	Walking,
	Running
};

UCLASS()
class GP4_TEAM7_API AGP_AICharacter : 
	public ACharacter, 
	public IGenericTeamAgentInterface
	//,public ICrowdAgentInterface
{
	GENERATED_BODY()

public:

	AGP_AICharacter(const FObjectInitializer& ObjInit);

protected:

	virtual void BeginPlay() override;
	virtual void BeginDestroy() override;

	UPROPERTY(VisibleAnywhere, Category = "AI|GenericTeamId")
	FGenericTeamId TeamID = 0;

public:	

	// for all enemies
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|BehaviorTree")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
	TObjectPtr<AGP_PatrolRouteActor> PatrolRoute;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
	bool bCanBeStunned = true;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Behavior")
	float StunnedTime = 2.f;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "AI|Movement")
	void SetMovementSpeed(ESpeedState SpeedState);

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "Generic Team Id")
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override { TeamID = NewTeamID; };

	UFUNCTION(BlueprintCallable, Category = "Generic Team Id")
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamID; }

	UFUNCTION(BlueprintCallable, Category = "AI|Behavior")
	void HandleDeath();

protected:

	UFUNCTION()
	void OnStartGlobalNormalTime();

	UFUNCTION()
	void OnStopGlobalNormalTime(float Time);

	AGP_AIController* AIController = nullptr;

	UPROPERTY(BlueprintReadWrite, Category = "AI|Behavior")
	float RemainedStunnedCooldownTime = 0.f;

	float TimeDilation = 1.0f;
};
