// Fill out your copyright notice in the Description page of Project Settings.


#include "MyPlayerController.h"
#include "Net/UnrealNetwork.h"

AMyPlayerController::AMyPlayerController(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	/* Initialize The Values */
	type = 1;

	/* Make sure the PawnClass is Replicated */
	bReplicates = true;
	DeterminePawnClass();
}

void AMyPlayerController::BeginPlay()
{
	Super::BeginPlay();

	
}

// Pawn Class
void AMyPlayerController::DeterminePawnClass_Implementation()
{
	if(1)
	//if (IsLocalController()) //Only Do This Locally (NOT Client-Only, since Server wants this too!)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Green, "DeterminePawnClass_Implementation");
		/* Use PawnA if the Text File tells us to */
		if (0)
		{
			ServerSetPawn(0);
			return;
		}

		/* Otherwise, Use PawnB :) */
		ServerSetPawn(1);
		return;
	}
}

bool AMyPlayerController::ServerSetPawn_Validate(int inType)
{
	return true;
}

void AMyPlayerController::ServerSetPawn_Implementation(int inType)
{
	type = inType;

	/* Just in case we didn't get the PawnClass on the Server in time... */
	//GetWorld()->GetAuthGameMode()->RestartPlayer(this);
}

// Replication
void AMyPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	DOREPLIFETIME(AMyPlayerController, type);
}
