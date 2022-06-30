// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileWeapon.h"


// Called only on server
void AProjectileWeapon::FireWeapon(const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	if (bShouldSpendBullet)
	{
		Super::FireWeapon(SpawnLocation, SpawnRotation);
	}

	if (!HasAuthority()) return;
	
	UWorld* World = GetWorld();
	if (ProjectileClass == nullptr ||  World == nullptr) return;

	FActorSpawnParameters SpawnParameters;
	// Set projectile owner and instigator to be this weapon's owner (the character who fired)
	SpawnParameters.Owner = GetOwner();
	APawn* InstigatorPawn = Cast<APawn>(GetOwner());
	if (InstigatorPawn)
	{
		SpawnParameters.Instigator = InstigatorPawn;
		
	}
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::
	AdjustIfPossibleButDontSpawnIfColliding;

	// Spawn projectile
	ProjectileCreated = World->SpawnActor<AActor>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParameters);
}
