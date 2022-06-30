// Fill out your copyright notice in the Description page of Project Settings.


#include "CombatComponent.h"

#include "Net/UnrealNetwork.h"
#include "PortalGameEnginesII/PortalGameEnginesIICharacter.h"
#include "PortalGameEnginesII/Weapon/PortalWeapon.h"

// Sets default values for this component's properties
UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, EquippedWeapons);
	DOREPLIFETIME(UCombatComponent, bIsReloading);
}

// Called when the game starts
void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

bool UCombatComponent::CanFire()
{
	if (EquippedWeapon == nullptr ||
		EquippedWeapon->GetCurrentAmmo() <= 0 || bIsReloading)
	{
		return false;
	}
	return true;
}

// Called by character when presses fire button (locally or server)
void UCombatComponent::FireButtonPressed()
{
	if (CanFire())
	{
		ServerFire();
	}
}

// Called only on server
void UCombatComponent::ServerFire_Implementation()
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;

	MulticastFire(EquippedWeapon->GetFireParticle());
}

// Called on server and all clients
void UCombatComponent::MulticastFire_Implementation(UParticleSystem* FireParticle)
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;
	
	Character->PlayFireEffects(FireParticle);
	// Fire weapon will only be called on server (checks for authority)
	EquippedWeapon->FireWeapon(Character->GetFireLocation(), Character->GetProjectileSpawnRotation());
}

// Called by character when presses right fire button (locally or server)
void UCombatComponent::RightFireButtonPressed()
{
	if (CanFire())
	{
		APortalWeapon* EquippedPortalWeapon = Cast<APortalWeapon>(EquippedWeapon);
		if (EquippedWeapon == nullptr) return;
		ServerAlternativeFire();
	}
}

// Called only on server
void UCombatComponent::ServerAlternativeFire_Implementation()
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;

	MulticastAlternativeFire(EquippedWeapon->GetFireParticle());
}

// Called on server and all clients
void UCombatComponent::MulticastAlternativeFire_Implementation(UParticleSystem* FireParticle)
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;

	// Portal is the only gun that supports alternative fire so we can cast directly to it.
	APortalWeapon* PortalWeaponEquipped = Cast<APortalWeapon>(EquippedWeapon);
	if (PortalWeaponEquipped == nullptr) return;
	
	Character->PlayFireEffects(FireParticle);
	// Fire weapon will only be called on server (checks for authority)
	PortalWeaponEquipped->FireAlternativeWeapon(Character->GetFireLocation(), Character->GetProjectileSpawnRotation());
}

// Called by character when presses reload button (locally or server)
void UCombatComponent::ReloadButtonPressed()
{
	if (EquippedWeapon == nullptr) return;
	
	if (EquippedWeapon->CanReload() && !bIsReloading)
	{
		ServerReload(EquippedWeapon, EquippedWeapon->GetTimeToReload());
	}
}

// Called only on server
void UCombatComponent::ServerReload_Implementation(AWeapon* WeaponToReload, float TimeToReload)
{
	if (Character == nullptr || WeaponToReload == nullptr) return;
	bIsReloading = true;
	MulticastIsReloading(WeaponToReload, TimeToReload);
	
	// Start timer to finish reloading
	FTimerHandle ReloadTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(ReloadTimerHandle, this, &UCombatComponent::OnReloadTimerFinished,
		EquippedWeapon->GetTimeToReload());
}

// Called only on server. Will replicate because bIsReloading is replicated
void UCombatComponent::OnReloadTimerFinished()
{
	bIsReloading = false;

	// Get bullets to reload
	if (EquippedWeapon == nullptr) return;
	const int32 SpaceInMag = EquippedWeapon->GetMaxAmmo() - EquippedWeapon->GetCurrentAmmo();
	const int32 Least = FMath::Min(SpaceInMag, EquippedWeapon->GetAdditionalAmmo());
	const int32 BulletsToReload = FMath::Clamp(SpaceInMag, 0, Least);

	// Update current ammo and additional ammo
	const int32 AdditionalBulletsToRemove = EquippedWeapon->GetAdditionalAmmo() - BulletsToReload;
	EquippedWeapon->SetAdditionalAmmo(AdditionalBulletsToRemove);
	EquippedWeapon->AddCurrentAmmoAndUpdateHUD(BulletsToReload);
}

// Called on server and all clients
void UCombatComponent::MulticastIsReloading_Implementation(AWeapon* WeaponToReload, float TimeToReload)
{
	if (Character == nullptr || WeaponToReload == nullptr) return;
	Character->PlayReloadAnimation(TimeToReload);
}

// Called by weapon only on server when character overlaps weapon
void UCombatComponent::AddEquippedWeapon(AWeapon* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr) return;
	
	EquippedWeapons.Add(WeaponToEquip);
	
	// Change equipped weapon to be this one
	EquippedWeapon = WeaponToEquip;
	
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	EquippedWeapon->SetOwner((AActor*)Character);

	// Update weapon ammo HUD
	EquippedWeapon->SetAmmoHUD();
	
	// Update weapon visuals (only on server)
	Character->UpdateEquippedWeaponColorAndHUD(EquippedWeapon->WeaponColor);
	// Hide weapon
	EquippedWeapon->SetActorHiddenInGame(true);
}

// Called only on server when character wants to change weapon
void UCombatComponent::SetEquippedWeapon(AWeapon* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr) return;

	EquippedWeapon = WeaponToEquip;
	Character->UpdateEquippedWeaponColorAndHUD(EquippedWeapon->WeaponColor);
}

// Called only on server when character overlaps a ammo pickup
void UCombatComponent::AddAmmoToEquippedWeapon(int32 AmmoToAdd)
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;

	EquippedWeapon->AddAdditionalAmmoAndUpdateHUD(AmmoToAdd);
}



