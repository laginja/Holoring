// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPCharacterPlayerController.h"

#include "HoloringCharacter.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCharacterDelegate);		// koristimo delegate kako bi mogli PlayerControlleru javiti da je characted umro. Moze se koristiti i kod Steam achievementa npr.

class UInputComponent;

UCLASS(config=Game)
class AHoloringCharacter : public ACharacter
{
	GENERATED_BODY()

private:
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	class USkeletalMeshComponent* Mesh1P;

	/** Pawn mesh: other player view (arms; seen only by self) */	// TODO; create whole body
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* Mesh3P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* FP_Gun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	class USceneComponent* FP_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(Replicated)
	FString Ime;

	UPROPERTY(EditDefaultsOnly, Category = "Setup")
	int StartingHealth = 100;

	UPROPERTY(VisibleAnywhere, Category = "Health", Replicated)
	int CurrentHealth = StartingHealth;

	AFPCharacterPlayerController* CharacterPlayerController = nullptr;

	UPROPERTY(VisibleAnywhere)
	FRotator AimRotation;

public:
	AHoloringCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(ReplicatedUsing = OnRep_ShotFired)
	uint32 bShotFired : 1;

	UFUNCTION()
	void OnRep_ShotFired();

	// Called by the engine when actor damage is dealt
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const & DamageEvent, class AController * EventInstigator, AActor * DamageCauser) override;

	// Returns current health as a percentage of starting health, between 0 and 1
	UFUNCTION(BlueprintPure, Category = "Health")
	float GetHealthPercent() const;

	// Used in FPCharacterPlayerController to set the PlayerController responsible for a character
	void SetThisPlayerController(AFPCharacterPlayerController* InPlayerController);

	// Return AFPCharacterPlayerController* to a blueprint which respawns the player
	UFUNCTION(BlueprintPure, Category = "Setup")
	AFPCharacterPlayerController* GetThisPlayerController() { return CharacterPlayerController; };

	// Delegate called when a player dies
	FCharacterDelegate OnDeath;

	// Delegate called when a player scores a kill
	FCharacterDelegate OnKill;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	uint32 bIsDead : 1;

	int GetCurrentHealth() { return CurrentHealth; };

	// Function for getting character pitch value
	FRotator GetViewRotation() const override;

	UFUNCTION(BlueprintPure)
	FRotator GetAimRotation();

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category=Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category=Projectile)
	TSubclassOf<class AHoloringProjectile> ProjectileClass;

	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	class USoundBase* FireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	class UAnimMontage* FireAnimation;

	/** Whether to use motion controller location for aiming. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint32 bUsingMotionControllers : 1;


	/** Returns Mesh1P subobject **/
	FORCEINLINE class USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }

protected:
	virtual void BeginPlay();

	/** Fires a projectile. */
	void OnFire();

	void PlayFireAnimation();

	void PlayFireSound();

	void SpawnProjectile();

	UFUNCTION(Server, Reliable, WithValidation)
	void Server_OnFire();


	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	 * Called via input to turn at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void TurnAtRate(float Rate);

	/**
	 * Called via input to turn look up/down at a given rate.
	 * @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	 */
	void LookUpAtRate(float Rate);
	
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

public:
	

};

