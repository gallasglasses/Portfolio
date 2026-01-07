// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GenericTeamAgentInterface.h"
#include "GP_AICharacter.generated.h"

class UBehaviorTree;
class UAnimMontage;
class UBoxComponent;
class UGP_HealthComponent;
class AGP_PatrolRoute;

DECLARE_MULTICAST_DELEGATE(FOnFinishedAttackSignature);
DECLARE_MULTICAST_DELEGATE(FOnFinishedAwakeSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnQTEStartedSignature);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnQTEFinishedSignature, bool, bSuccess);

UENUM(BlueprintType)
enum class EAIAnimState : uint8
{
	None,
	Sleep,
	StandUp,
	Idle,
	Walk,
	Attack,
	Death
};

UCLASS()
class GP3_TEAM10_API AGP_AICharacter : public ACharacter, public IGenericTeamAgentInterface
{
	GENERATED_BODY()

public:

	AGP_AICharacter();

protected:

	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Animations")
	UAnimMontage* AttackAnimMontage;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Animations")
	UAnimMontage* DeathAnimMontage;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "AI|Components")
	UGP_HealthComponent* HealthComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Attack", meta = (ClampMin = "0", ClampMax = "1000.0"))
	float DamageAmount;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior", meta = (ClampMin = "0", ClampMax = "100.0"))
	float GrabProbability = 30.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Behavior", meta = (ClampMin = "0", ClampMax = "10000.0"))
	float PushingForce = 5000.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|Behavior", meta = (ClampMin = "0", ClampMax = "1000.0"))
	float DealtDamageAfterQTE = 35.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|HitImpactShader")
	float HitImpactShaderFadeDuration = 0.5f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|HitImpactShader", meta = (ClampMin = "0.01", ClampMax = "1.0"))
	float HitImpactShaderFadeRateTime = 0.05f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|HitImpactShader")
	int32 MaxHitForCumulativeImpactShader = 1;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|HitImpactShader", meta = (ClampMin = "0.4", ClampMax = "2.0"))
	float ThresholdForCumulativeImpactShader = 0.55f;

	UPROPERTY(VisibleAnywhere, Category = "AI|GenericTeamId")
	FGenericTeamId TeamID = 0;

	UPROPERTY()
	UMaterialInstanceDynamic* OverlayMaterialInstance = nullptr;

public:

	FOnFinishedAttackSignature OnFinishedAttack;
	FOnFinishedAwakeSignature OnFinishedAwake;
	FOnQTEStartedSignature OnQTEStarted;

	UPROPERTY(BlueprintAssignable)
	FOnQTEFinishedSignature OnQTEFinished;

	UFUNCTION(BlueprintCallable, Category = "AI|Behavior")
	void StartQTE(AActor* PlayerToDamage);

	UFUNCTION(BlueprintCallable, Category = "AI|Behavior")
	void FinishQTE();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI|BehaviorTree")
	TObjectPtr<UBehaviorTree> BehaviorTree;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI|Behavior")
	TObjectPtr<AGP_PatrolRoute> PatrolRoute;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Behavior")
	EAIAnimState StartAnimState = EAIAnimState::Sleep;

	UFUNCTION(BlueprintCallable, Category = "AI|Behavior")
	bool CanAwake() const { return bCanAwaken; };

	UFUNCTION(BlueprintCallable, Category = "AI|Behavior")
	bool IsAwakening() const { return bIsAwakening; };

	UFUNCTION(BlueprintCallable, Category = "AI|Behavior")
	void SetIsAwakening(bool IsAwakening) { bIsAwakening = IsAwakening; };

	UFUNCTION(BlueprintCallable, Category = "AI|Behavior")
	bool IsDamageDone() const { return bIsDamageDone; };

	UFUNCTION(BlueprintCallable, Category = "AI|Behavior")
	float GetDamage() const { return DamageAmount; };

	UFUNCTION(BlueprintCallable, Category = "AI|Behavior")
	float GetGrabProbability() const { return GrabProbability; };

	UFUNCTION(BlueprintCallable, Category = "AI|Behavior")
	float GetPushingForce() const { return PushingForce; };

	UFUNCTION(BlueprintCallable, Category = "AI|Behavior")
	float GetDealtDamageAfterQTE() const { return DealtDamageAfterQTE; };

	UFUNCTION(BlueprintCallable, Category = "AI|Behavior")
	void SetIsDamageDone(bool IsDamageDone) { bIsDamageDone = IsDamageDone; };

	UFUNCTION(BlueprintCallable, Category = "AI|Behavior")
	EAIAnimState GetCurrentAnimState() const { return CurrentAnimState; };

	UFUNCTION(BlueprintCallable, Category = "AI|Behavior")
	void SetCurrentAnimState(EAIAnimState NextAnimState);

	UFUNCTION(BlueprintCallable, Category = "Generic Team Id")
	virtual void SetGenericTeamId(const FGenericTeamId& NewTeamID) override { TeamID = NewTeamID; };

	UFUNCTION(BlueprintCallable, Category = "Generic Team Id")
	virtual FGenericTeamId GetGenericTeamId() const override { return TeamID; }

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION(BlueprintCallable, Category = "AI|Attack")
	void Attack();

	void StopAttack();
	void FinishAwake();

private:

	bool bQTEActive = false;
	AActor* QTETarget = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "AI|QTE")
	float QTEAttackRateTime = 1.0f;

	bool bIsDamageDone = false;

	FTimerHandle AttackTimerHandle;
	FTimerHandle AnimationTimerHandle;
	FTimerHandle HitImpactShaderTimerHandle;

	UPROPERTY(EditDefaultsOnly, Category = "AI|Behavior")
	bool bCanAwaken = false;

	UPROPERTY(VisibleAnywhere, Category = "AI|Behavior")
	bool bIsAwakening = false;

	EAIAnimState CurrentAnimState = EAIAnimState::None;

	UFUNCTION()
	void HandleDeath();

	UFUNCTION()
	void HandleDamage(float Damage, const UDamageType* DamageType, AController* InstigatedBy);

	void HandleQTEAttack();

	UFUNCTION()
	void HandleAnimBlendOut(UAnimMontage* AnimMontage, FName InSectionName, float BlendOutStartTime);

	float GetMontageSectionLength(UAnimMontage* AnimMontage, FName InSectionName);

	UFUNCTION()
	void HandleNextAnimation(FName InSectionName);

	UFUNCTION()
	void StartImpactShaderFade(UMaterialInstanceDynamic* Material);

	UFUNCTION()
	void UpdateImpactShaderFade(UMaterialInstanceDynamic* Material, float StepAmount, float CurrentIntensityImpactShader);

	size_t CurrentImpactShaderStep = 0;
	int32 HitCount = 0;
	int32 PreviousType = -1;
	float ElapsedTimeBetweenHit = 0.0f;
};
