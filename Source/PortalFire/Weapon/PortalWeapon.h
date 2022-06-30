// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProjectileWeapon.h"
#include "PortalWeapon.generated.h"

/**
 * 
 */
UCLASS()
class PORTALGAMEENGINESII_API APortalWeapon : public AProjectileWeapon
{
	GENERATED_BODY()

public:
	// Called by combat component.
	void FireAlternativeWeapon(const FVector& MuzzleLocation, const FRotator& SpawnRotation);
protected:
	virtual void FireWeapon(const FVector& MuzzleLocation, const FRotator& SpawnRotation) override;
	// Called when player uses the right mouse button to fire.
	// Only available on portal gun
};
