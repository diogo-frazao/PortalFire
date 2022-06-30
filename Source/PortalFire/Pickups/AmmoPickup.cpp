// Fill out your copyright notice in the Description page of Project Settings.


#include "AmmoPickup.h"

#include "PortalGameEnginesII/PortalGameEnginesIICharacter.h"
#include "PortalGameEnginesII/Weapon/Ammo.h"

void AAmmoPickup::OnPickupOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const APortalGameEnginesIICharacter* Character = Cast<APortalGameEnginesIICharacter>(OtherActor);
	if (Character == nullptr) return;
	PickupOverlapAction();	
}

void AAmmoPickup::PickupOverlapAction()
{
	if (SpawnedClass == nullptr) return;

	const AAmmo* SpawnedAmmo = Cast<AAmmo>(SpawnedClass); 
	if (SpawnedAmmo == nullptr) return;

	if (!bIsAlreadyRestartingSpawn)
	{
		bIsAlreadyRestartingSpawn = true;

		FTimerHandle RespawnTimerHandle;
		GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &AAmmoPickup::OnRespawnTimerFinished,
	TimeToRespawn);
	}
}

void AAmmoPickup::OnRespawnTimerFinished()
{
	const FVector WeaponSpawnLocation(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 50.f);
	CreateNewSpawnedClass();
	bIsAlreadyRestartingSpawn = false;
}

