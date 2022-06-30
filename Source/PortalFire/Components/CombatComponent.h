// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PortalGameEnginesII/Weapon/Weapon.h"
#include "CombatComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PORTALGAMEENGINESII_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UCombatComponent();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	// Let character have access to all combat component properties
	friend class APortalGameEnginesIICharacter;

	void AddEquippedWeapon(AWeapon* WeaponToEquip);
	void SetEquippedWeapon(AWeapon* WeaponToEquip);

	/** Called when player overlaps an ammo pickup */
	void AddAmmoToEquippedWeapon(int32 AmmoToAdd);
	
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	// Called by character when presses the fire button
	UFUNCTION(BlueprintCallable, Category = "Fire")
	void FireButtonPressed();

	// Called by character when presses right fire button
	UFUNCTION(BlueprintCallable, Category = "Fire")
	void RightFireButtonPressed();

	// Called by character when presses the reload button
	UFUNCTION(BlueprintCallable, Category = "Fire")
	void ReloadButtonPressed();

	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bIsReloading;

	UFUNCTION(Server, Reliable)
	void ServerReload(AWeapon* WeaponToReload, float TimeToReload);

	void OnReloadTimerFinished();

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastIsReloading(AWeapon* WeaopnToReload, float TimeToReload);

	// Called Called by character when presses the fire button
	// Used to call a multicast to play fire effects
	UFUNCTION(Server, Reliable)
	void ServerFire();
	
	// Called Called by character when presses the right fire button
	// Used to call a multicast to play fire effects
	UFUNCTION(Server, Reliable)
	void ServerAlternativeFire();

	UFUNCTION(BlueprintCallable)
	bool CanFire();

	/** Called to play fire effects (sounds & muzzle flash) */
	/** Multicast RPC to make effects visible on all clients and server */
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(UParticleSystem* FireParticle);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastAlternativeFire(UParticleSystem* FireParticle);

private:

	APortalGameEnginesIICharacter* Character;

	/** Array of equipped weapons */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Weapons", meta = (AllowPrivateAccess = "true"))
	TArray<AWeapon*> EquippedWeapons;

	/** Current equipped weapon */
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Weapons", meta = (AllowPrivateAccess = "true"))
	AWeapon* EquippedWeapon;
	
public:
	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
};
