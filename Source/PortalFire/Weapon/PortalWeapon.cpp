// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalWeapon.h"

#include "PortalGameEnginesII/PortalGameEnginesIIProjectile.h"

void APortalWeapon::FireWeapon(const FVector& MuzzleLocation, const FRotator& SpawnRotation)
{
	Super::FireWeapon(MuzzleLocation, SpawnRotation);

	if (!HasAuthority()) return;

	if (ProjectileCreated == nullptr) return;
	APortalGameEnginesIIProjectile* PortalProjectile = Cast<APortalGameEnginesIIProjectile>(ProjectileCreated);
	if (PortalProjectile == nullptr) return;
	PortalProjectile->SetShouldSpawnPortalBlue(true);
}

void APortalWeapon::FireAlternativeWeapon(const FVector& MuzzleLocation, const FRotator& SpawnRotation)
{
	Super::FireWeapon(MuzzleLocation, SpawnRotation);

	if (!HasAuthority()) return;
	
	if (ProjectileCreated == nullptr) return;
	APortalGameEnginesIIProjectile* PortalProjectile = Cast<APortalGameEnginesIIProjectile>(ProjectileCreated);
	if (PortalProjectile == nullptr) return;
	PortalProjectile->SetShouldSpawnPortalBlue(false);
}


