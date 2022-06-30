// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponPickup.h"

#include "PortalGameEnginesII/PortalGameEnginesIICharacter.h"

void AWeaponPickup::OnPickupOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                    UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	const APortalGameEnginesIICharacter* Character = Cast<APortalGameEnginesIICharacter>(OtherActor);
	if (Character == nullptr) return;
	PickupOverlapAction();	
}

void AWeaponPickup::PickupOverlapAction()
{
	if (SpawnedClass == nullptr) return;

	const AWeapon* SpawnedWeapon = Cast<AWeapon>(SpawnedClass);
	if (SpawnedWeapon == nullptr) return;
	
	// If I've overlapped the pickup and the weapon on this spawner
	if (SpawnedWeapon->GetWeaponState() == EWeaponState::EWS_Equipped && !bIsAlreadyRestartingSpawn)
	{
		// Spawn another weapon in here after x seconds
		// Avoid restarting timer every time the player overlaps the pickup spawn
		bIsAlreadyRestartingSpawn = true;

		if (bCanOnlyRespawnAfterOwnerDies == false)
		{
			FTimerHandle RespawnTimerHandle;
			GetWorldTimerManager().SetTimer(RespawnTimerHandle, this, &AWeaponPickup::OnRespawnTimerFinished,
				TimeToRespawn);	
		}
	}
}

void AWeaponPickup::OnRespawnTimerFinished()
{
	const FVector WeaponSpawnLocation(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 50.f);
	CreateNewSpawnedClass();
	bIsAlreadyRestartingSpawn = false;
}

