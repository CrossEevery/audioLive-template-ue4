// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameMode.h"

void AMyGameMode::GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList)
{
    ActorList = K2_OnGetSeamlessTravelActorList(bToTransition, ActorList);
    Super::GetSeamlessTravelActorList(bToTransition, ActorList);
}