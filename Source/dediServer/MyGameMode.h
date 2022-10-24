// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "MyGameMode.generated.h"

/**
 * 
 */
UCLASS()
class DEDISERVER_API AMyGameMode : public AGameMode
{
	GENERATED_BODY()
	
public:
    void GetSeamlessTravelActorList(bool bToTransition, TArray<AActor*>& ActorList);

    /**
    * Overridable event for GameMode blueprint to respond to a change name call
    * @param bToTransition    How are we transitioning
    * @param ActorList        Add actors to this list to seamless travel with.
    */
    UFUNCTION(BlueprintImplementableEvent, Category = Game, meta = (DisplayName = "GetSeamlessTravelActorList", ScriptName = "GetSeamlessTravelActorList"))
        TArray<AActor*> K2_OnGetSeamlessTravelActorList(bool bToTransitionOut, UPARAM(ref)TArray<AActor*>& ActorListOut);
};
