// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class DEDISERVER_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	/* Constructor */
	AMyPlayerController(const FObjectInitializer& ObjectInitializer);

	FORCEINLINE int GetPlayerPawnClass() { return type; }

protected:
	/* Return The Correct Pawn Class Client-Side */
	UFUNCTION(Reliable, Client)
		void DeterminePawnClass();
	virtual void DeterminePawnClass_Implementation();

	/* Use BeginPlay to start the functionality */
	virtual void BeginPlay() override;

	/* Set Pawn Class On Server For This Controller */
	UFUNCTION(Reliable, Server, WithValidation)
		virtual void ServerSetPawn(int inType);
	virtual void ServerSetPawn_Implementation(int inType);
	virtual bool ServerSetPawn_Validate(int inType);

	/* Actual Pawn class we want to use */
	UPROPERTY(BlueprintReadOnly, Replicated)
		int type;

};
