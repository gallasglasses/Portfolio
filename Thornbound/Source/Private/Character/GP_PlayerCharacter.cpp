// Fill out your copyright notice in the Description page of Project Settings.


#include "Character/GP_PlayerCharacter.h"
#include "Engine/DamageEvents.h"
#include "AI/GP_AICharacter.h"
#include "Core/GP_GameModeBase.h"
#include "Components/GP_QTEComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

DEFINE_LOG_CATEGORY_STATIC(GP_PlayerCharacterLog, All, All);

AGP_PlayerCharacter::AGP_PlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	QTEComponent = CreateDefaultSubobject<UGP_QTEComponent>("QTEComponent");
}

void AGP_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	check(QTEComponent);
}

void AGP_PlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AGP_PlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AGP_PlayerCharacter::StartQTE_Implementation(AActor* InstigatorActor)
{
	if (!GetWorld() || !GetWorld()->GetAuthGameMode()) return;
	const auto GameMode = Cast<AGP_GameModeBase>(GetWorld()->GetAuthGameMode());
	if (GameMode)
	{
		GameMode->StartQTEState();
	}

	// start QTE component
	UE_LOG(GP_PlayerCharacterLog, Display, TEXT("%s started QTE by instigator %s"), *GetName(), *InstigatorActor->GetName());
	QTEComponent->StartQTE(InstigatorActor);
	GetCharacterMovement()->DisableMovement();
	//EnableInput(GetWorld()->GetFirstPlayerController());
}

