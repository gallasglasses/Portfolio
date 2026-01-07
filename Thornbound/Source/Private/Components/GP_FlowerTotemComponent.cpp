// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/GP_FlowerTotemComponent.h"

UGP_FlowerTotemComponent::UGP_FlowerTotemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UGP_FlowerTotemComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UGP_FlowerTotemComponent::OnPickedUp_Implementation()
{
	OnFlowerTotemPickedUp.Broadcast();
}

FOnFlowerTotemPickedUpSignature& UGP_FlowerTotemComponent::GetFlowerTotemPickedUpDelegate()
{
	return OnFlowerTotemPickedUp;
}