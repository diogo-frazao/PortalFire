// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/CombatComponent.h"
#include "GameFramework/Character.h"
#include "PlayerController/PortalPlayerController.h"
#include "Weapon/Weapon.h"
#include "PortalGameEnginesIICharacter.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UAnimMontage;
class USoundBase;

UCLASS(config=Game)
class APortalGameEnginesIICharacter : public ACharacter
{
	GENERATED_BODY()

public:
	APortalGameEnginesIICharacter();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;

	/** Play reload animation */
	UFUNCTION(BlueprintImplementableEvent)
	void PlayReloadAnimation(float AnimationTime);

	/** Called when player equips a new weapon */
	void EquipWeapon(AWeapon* Weapon);

	/** Called when the owning player wants to change the equipped weapon */
	UFUNCTION(Server, Reliable)
	void ServerSetWeaponToEquip(AWeapon* WeaponToEquip);

	/** Function called only by server when the player equips a new weapon */
	UFUNCTION(BlueprintImplementableEvent, Category = "Weapons")
	void UpdateEquippedWeaponColorAndHUD(FLinearColor NewColor);

	/** Blueprint function called on server and all clients to player fire effects */
	UFUNCTION(BlueprintImplementableEvent, Category = "Weapons")
	void PlayFireEffects(UParticleSystem* FireParticle);

	/** Function called only on the server to handle player elimination */
	UFUNCTION(BlueprintCallable, Category = "Death")
	void PlayerEliminated();
	
	/** What happens when the player is eliminated. Called on server and all clients *
	 * @param bIsReset if true will reset the effects
	 */
	UFUNCTION(NetMulticast, Reliable, BlueprintCallable, Category = "Death")
	void MulticastPlayerEliminatedEffects(bool bIsReset);
	
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
	TSubclassOf<class APortalGameEnginesIIProjectile> ProjectileClass;

	/** Blue Portal Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* BluePortalFireSound;

	/** Orange Portal Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
	USoundBase* OrangePortalFireSound;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	UAnimMontage* FireAnimation;

	/** Get equipped weapon index */
	int32 GetEquippedWeaponIndex() const;

	/** Change whether the character can walk or not */
	// Used to disable movement when all players are not connected to the lobby
	UFUNCTION(BlueprintCallable)
	void SetCanWalk(bool Value);

protected:
	
    virtual void BeginPlay() override;
	/** Fires a projectile. */
	APortalGameEnginesIIProjectile* OnFire(USoundBase* FireSound);

	UFUNCTION(BlueprintCallable)
	void FireButtonPressed();

	UFUNCTION(BlueprintCallable)
	void ReloadButtonPressed();
	
	/** Tries to spawn Portal B */
	UFUNCTION(BlueprintCallable, Category = "PortalSpawn")
	void FirePortalOrange();

	/**
	 * Player Movement
	 */
	void MoveForward(float Val);
	void MoveRight(float Val);
	void TurnAtRate(float Rate);
	void LookUpAtRate(float Rate);
	void ChangeToNextWeapon();
	void ChangeToPreviousWeapon();

	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly)
	bool bCanWalk = true;

	// Called only on server when character is damaged
	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);
	
	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerDeath(APortalGameEnginesIICharacter* EliminatedCharacter);
	
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	/** Variable used to know if the player is alive or not */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	bool bIsAlive = true;
	
private:
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category=Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh1P;

	/** Gun mesh: 1st person view (seen only by self) */
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* FP_Gun;

	/** Gun mesh: 3rd person view (seen only by others) */
	UPROPERTY(BlueprintReadOnly, VisibleDefaultsOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	USkeletalMeshComponent* TpGun;

	/** Location on gun mesh where projectiles should spawn. */
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = Mesh, meta = (AllowPrivateAccess = "true"))
	USceneComponent* FP_MuzzleLocation;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FirstPersonCameraComponent;

	/** Combat component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UCombatComponent* CombatComponent;

	/**
	 * Player Health
	 */
	
	/** Max health the player can have */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float MaxHealth = 100.f;

	/** Current health of the player. Replicated to all clients */
	UPROPERTY(ReplicatedUsing = OnRep_CurrentHealth, VisibleAnywhere,BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	float CurrentHealth = 100.f; 

	// Called on all clients
	UFUNCTION()
	void OnRep_CurrentHealth();

	/**
	 * Player Respawn
	 */
	/** Delay to respawn the player after his death */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Death", meta = (AllowPrivateAccess = "true"))
	float PlayerRespawnDelay = 5.f;

	/** Function called after the PlayerRespawnDelay. Called only on server */
	UFUNCTION(BlueprintCallable, Category = "Death")
	void OnPlayerRespawnTimerFinished();

	FTimerHandle PlayerRespawnTimerHandle;

	/** Reference to this character's player controller */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	APortalPlayerController* PlayerController;

	/**
	 * Player Kills
	 */

	/** Function called after the AccessPlayerStateDelay.*/
	UFUNCTION(BlueprintCallable, Category = "Stats")
	void OnPlayerStateTimerFinished();
	
	FTimerHandle PlayerStateTimerHandle;

	
public:
	/** Returns Mesh1P subobject **/
	FORCEINLINE USkeletalMeshComponent* GetMesh1P() const { return Mesh1P; }
	/** Returns FirstPersonCameraComponent subobject **/
	FORCEINLINE UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	/** Get combat component */
	FORCEINLINE UCombatComponent* GetCombatComponent() const { return CombatComponent; }
	/** Returns the location to fire a projectile at */
	UFUNCTION(BlueprintPure, Category = Mesh)
	FVector GetFireLocation() const;
	FORCEINLINE FRotator GetProjectileSpawnRotation() const { return GetControlRotation(); }
	FORCEINLINE float GetCurrentHealth() const { return CurrentHealth; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE APortalPlayerController* GetPlayerController() const { return PlayerController; }
};

