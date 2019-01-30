// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "HoloringCharacter.h"
#include "HoloringProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"

#include "UnrealNetwork.h"
#include "DrawDebugHelpers.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// AHoloringCharacter

AHoloringCharacter::AHoloringCharacter()
{
	if (HasAuthority()) {
		this->SetReplicates(true);
	}

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->RelativeLocation = FVector(-39.56f, 1.75f, 64.f); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->RelativeRotation = FRotator(1.9f, -19.19f, 5.2f);
	Mesh1P->RelativeLocation = FVector(-0.5f, -4.4f, -155.7f);

	// Create a mesh component that will be used when being viewed from other player's view (when not controlling this pawn)
	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh3P"));
	Mesh3P->SetOnlyOwnerSee(false);
	Mesh3P->SetupAttachment(FirstPersonCameraComponent);
	Mesh3P->bCastDynamicShadow = false;
	Mesh3P->CastShadow = false;

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	// FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetupAttachment(RootComponent);

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for Mesh1P, FP_Gun, and VR_Gun 
	// are set in the derived blueprint asset named MyCharacter to avoid direct content references in C++.

	// Uncomment the following line to turn motion controllers on by default:
	//bUsingMotionControllers = true;

	Ime = "Matko";
	bShotFired = false;
	bIsDead = false;
}

void AHoloringCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show or hide the two versions of the gun based on whether or not we're using motion controllers.
	if (bUsingMotionControllers)
	{
		Mesh1P->SetHiddenInGame(true, true);
	}
	else
	{
		Mesh1P->SetHiddenInGame(false, true);
	}
}

void AHoloringCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	DrawDebugString(GetWorld(), FVector(0, 0, 100), Ime, this, FColor::White, DeltaTime);

	FirstPersonCameraComponent->SetWorldRotation(GetViewRotation());
	AimRotation = GetViewRotation();
}

void AHoloringCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const		// ne mijenjaj ime argumenta
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);		// potrebno je pozvati 'Super' metodu kako bi ispravno radili ispisi actor roles
	DOREPLIFETIME(AHoloringCharacter, bShotFired);				// ovaj makro prihvaca varijablu za replikaciju. Replicira 'bShotFired'; kad god se ta varijabla postavi na serveru, svi klijenti ce vidjeti tu vrijednost
																// i moci postaviti svoju vrijednost transformacije na tu vrijednost
	DOREPLIFETIME(AHoloringCharacter, Ime);
	DOREPLIFETIME(AHoloringCharacter, CurrentHealth);
}

void AHoloringCharacter::OnRep_ShotFired()			// izvrsava se samo na klijentu pa server ni ne vidi nikakve promjene
{
	UE_LOG(LogTemp, Warning, TEXT("OnRep_ShouldPlay() fired!"));
	PlayFireAnimation();
	PlayFireSound();
}

float AHoloringCharacter::TakeDamage(float DamageAmount, FDamageEvent const & DamageEvent, AController * EventInstigator, AActor * DamageCauser)
{
	int32 DamagePoints = FPlatformMath::RoundToInt(DamageAmount);
	int32 DamageToApply = FMath::Clamp(DamagePoints, 0, CurrentHealth);
	
	CurrentHealth -= DamageToApply;
	if (CurrentHealth <= 0)
	{
		auto Eliminator = Cast<AHoloringCharacter>(EventInstigator->GetPawn());
		Eliminator->OnKill.Broadcast();
		UE_LOG(LogTemp, Warning, TEXT("I got killed by: %s"), *EventInstigator->GetPawn()->GetName());
		
		bIsDead = true;
		OnDeath.Broadcast();
	}
	
	return DamageToApply;
}


//////////////////////////////////////////////////////////////////////////
// Input

void AHoloringCharacter::SetThisPlayerController(AFPCharacterPlayerController * InPlayerController)
{
	if (InPlayerController)
	{
		CharacterPlayerController = InPlayerController;
	}
}

void AHoloringCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AHoloringCharacter::OnFire);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AHoloringCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AHoloringCharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AHoloringCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AHoloringCharacter::LookUpAtRate);
}

void AHoloringCharacter::OnFire()
{
	if (Role == ROLE_AutonomousProxy)
	{
		//UE_LOG(LogTemp, Warning, TEXT("Client shooting"));
		Server_OnFire();
	}
	else if (Role == ROLE_Authority)
	{
		UE_LOG(LogTemp, Warning, TEXT("Server shooting"));
		PlayFireSound();
		PlayFireAnimation();
		SpawnProjectile();
		bShotFired = !bShotFired;
	}
}

void AHoloringCharacter::Server_OnFire_Implementation()
{
	SpawnProjectile();
	PlayFireAnimation();
	PlayFireSound();
	bShotFired = !bShotFired;
}

void AHoloringCharacter::PlayFireSound()
{
	// try and play the sound if specified
	if (FireSound != NULL)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}
}

void AHoloringCharacter::PlayFireAnimation()   
{
	// try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
		
		if (AnimInstance != NULL)
		{
			UE_LOG(LogTemp, Warning, TEXT("Animation Played"));
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
}

bool AHoloringCharacter::Server_OnFire_Validate()
{
	return true; 
}

void AHoloringCharacter::SpawnProjectile()
{
	// try and fire a projectile
	if (ProjectileClass != NULL)
	{
		UWorld* const World = GetWorld();
		if (World != NULL)
		{
			if (bUsingMotionControllers)
			{
				// Using motion controllers
			}
			else
			{
				const FRotator SpawnRotation = GetControlRotation();
				// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
				const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

				//Set Spawn Collision Handling Override
				FActorSpawnParameters ActorSpawnParams;
				ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

				// spawn the projectile at the muzzle
				auto Projectile = World->SpawnActor<AHoloringProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, ActorSpawnParams);
				
				// Set the owner of projectile
				if (Projectile)
				{
					Projectile->SetProjectileOwner(this);
				}
			}
		}
	}
}

float AHoloringCharacter::GetHealthPercent() const
{
	return (float)CurrentHealth / (float)StartingHealth;
}

FRotator AHoloringCharacter::GetViewRotation() const
{
	// if we have a controller we are on a server on an owning client
	if (Controller)
	{
		return Controller->GetControlRotation();
	}
		
	// else we are on a non owning client
	return FRotator(RemoteViewPitch / 255.f * 360.f, GetActorRotation().Yaw, 0.f);
}

FRotator AHoloringCharacter::GetAimRotation()
{
	return AimRotation;
}





// Inherited movement

void AHoloringCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void AHoloringCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AHoloringCharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AHoloringCharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}