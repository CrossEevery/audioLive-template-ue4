// Copyright Epic Games, Inc. All Rights Reserved.

#include "dediServerCharacter.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Controller.h"
#include "GameFramework/SpringArmComponent.h"
#include "Net/UnrealNetwork.h" 
//////////////////////////////////////////////////////////////////////////
// AdediServerCharacter

AdediServerCharacter::AdediServerCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
	GetCharacterMovement()->MaxWalkSpeed = 6000.f;
	//GetCapsuleComponent()->InitCapsuleSize(150.f, 300.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate =  45.f;

	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true; // Character moves in the direction of input...	
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 540.0f, 0.0f); // ...at this rotation rate
	GetCharacterMovement()->JumpZVelocity = 600.f;
	GetCharacterMovement()->AirControl = 0.2f;

	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 300.0f; // The camera follows at this distance behind the character	
	CameraBoom->bUsePawnControlRotation = true; // Rotate the arm based on the controller

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName); // Attach the camera to the end of the boom and let the boom adjust to match the controller orientation
	FollowCamera->bUsePawnControlRotation = false; // Camera does not rotate relative to arm

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named MyCharacter (to avoid direct content references in C++)
}

//////////////////////////////////////////////////////////////////////////
// Input

void AdediServerCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up gameplay key bindings
	check(PlayerInputComponent);
	FInputActionBinding& NewBinding1 = PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	NewBinding1.bConsumeInput = false;
	
	FInputActionBinding&  NewBinding2 = PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	NewBinding2.bConsumeInput = false;
	
	FInputAxisBinding& NewAxisBinding1 = PlayerInputComponent->BindAxis("MoveForward", this, &AdediServerCharacter::MoveForward);
	NewAxisBinding1.bConsumeInput = false;
	
	FInputAxisBinding& NewAxisBinding2 = PlayerInputComponent->BindAxis("MoveRight", this, &AdediServerCharacter::MoveRight);
	NewAxisBinding2.bConsumeInput = false;
	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	FInputAxisBinding& NewAxisBinding3 = PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	NewAxisBinding3.bConsumeInput = false;
	
	FInputAxisBinding& NewAxisBinding4 = PlayerInputComponent->BindAxis("TurnRate", this, &AdediServerCharacter::TurnAtRate);
	NewAxisBinding4.bConsumeInput = false;
	
	FInputAxisBinding& NewAxisBinding5 = PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	NewAxisBinding5.bConsumeInput = false;
	
	FInputAxisBinding& NewAxisBinding6 = PlayerInputComponent->BindAxis("LookUpRate", this, &AdediServerCharacter::LookUpAtRate);
	NewAxisBinding6.bConsumeInput = false;

	// handle touch devices
	PlayerInputComponent->BindTouch(IE_Pressed, this, &AdediServerCharacter::TouchStarted);
	PlayerInputComponent->BindTouch(IE_Released, this, &AdediServerCharacter::TouchStopped);

	// VR headset functionality
	PlayerInputComponent->BindAction("ResetVR", IE_Pressed, this, &AdediServerCharacter::OnResetVR);
}


void AdediServerCharacter::OnResetVR()
{
	// If dediServer is added to a project via 'Add Feature' in the Unreal Editor the dependency on HeadMountedDisplay in dediServer.Build.cs is not automatically propagated
	// and a linker error will result.
	// You will need to either:
	//		Add "HeadMountedDisplay" to [YourProject].Build.cs PublicDependencyModuleNames in order to build successfully (appropriate if supporting VR).
	// or:
	//		Comment or delete the call to ResetOrientationAndPosition below (appropriate if not supporting VR)
	UHeadMountedDisplayFunctionLibrary::ResetOrientationAndPosition();
}

void AdediServerCharacter::TouchStarted(ETouchIndex::Type FingerIndex, FVector Location)
{
		Jump();
}

void AdediServerCharacter::TouchStopped(ETouchIndex::Type FingerIndex, FVector Location)
{
		StopJumping();
}

void AdediServerCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AdediServerCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

void AdediServerCharacter::MoveForward(float Value)
{
	if ((Controller != nullptr) && (Value != 0.0f))
	{
		// find out which way is forward
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AdediServerCharacter::MoveRight(float Value)
{
	if ( (Controller != nullptr) && (Value != 0.0f) )
	{
		// find out which way is right
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);
	
		// get right vector 
		const FVector Direction = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		// add movement in that direction
		AddMovementInput(Direction, Value);
	}
}

void AdediServerCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to every client
	//
	DOREPLIFETIME(AdediServerCharacter, skill);
} 