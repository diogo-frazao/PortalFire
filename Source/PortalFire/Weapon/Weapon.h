// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8
{
	EWS_Initial,
	EWS_Equipped,
	EWS_Dropped,
	EWS_MAX
};

UCLASS()
class PORTALGAMEENGINESII_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AWeapon();
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void OnRep_Owner() override;

	/** Adds the amount to the weapon's current ammo */
	void AddCurrentAmmoAndUpdateHUD(int32 Amount);

	/** Adds the amount to the weapon's additional ammo */
	void AddAdditionalAmmoAndUpdateHUD(int32 Amount);

	/** Set Weapon State */
	void SetWeaponState(EWeaponState NewState);

	/** Called when weapon state changes. Called only on all clients */
	UFUNCTION()
	void OnRep_WeaponState();

	/** Weapon body color to be set when this weapon is equipped */
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties")
	FLinearColor WeaponColor;

	/** Fire the weapon */
	UFUNCTION(BlueprintCallable, Category = "Fire")
	virtual void FireWeapon(const FVector& MuzzleLocation, const FRotator& SpawnRotation);

	/** Function called to update the HUD to mach this weapon's bullets */
	UFUNCTION(BlueprintCallable)
	void SetAmmoHUD();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	/** Gun's mesh */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties")
	USkeletalMeshComponent* WeaponMesh;

	/** Sphere collider where player can pickup weapon */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Weapon Properties")
	USphereComponent* WeaponCollectionSphere;

	/** Weapon muzzle flash effect*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties")
	UParticleSystem* FireParticle;

	/** Method called when player overlapped with weapon collection sphere */
	UFUNCTION()
	virtual void OnWeaponOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);

	/** Current ammo the weapon has */
	UPROPERTY(ReplicatedUsing = OnRep_CurrentAmmo, EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties")
	int32 CurrentAmmo;

	// Called on all clients when current weapon changes
	UFUNCTION()
	void OnRep_CurrentAmmo();
	
	/** Max ammo the weapon has per mag */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties")
	int32 MaxAmmo;

	/** Time it takes for the weapon to reload */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties")
	float TimeToReload = 0.2f;

	/** Additional ammo the weapon has */
	UPROPERTY(ReplicatedUsing = OnRep_AdditionalAmmo, EditAnywhere, BlueprintReadOnly, Category = "Weapon Properties")
	int32 AdditionalAmmo;

	// Called on all clients when additional weapon ammo changes
	UFUNCTION()
	void OnRep_AdditionalAmmo();
	
	UFUNCTION(BlueprintCallable)
	void SpendBullet();
private:

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "Weapon Properties")
	EWeaponState WeaponState;

	class APortalGameEnginesIICharacter* CharacterRef = nullptr;
	class APortalPlayerController* ControllerRef = nullptr;

public:
	FORCEINLINE UParticleSystem* GetFireParticle() const { return FireParticle; }
	
	// Called by game mode when respawning weapon
	FORCEINLINE void SetWeaponColor(FLinearColor NewWeaponColor) { WeaponColor = NewWeaponColor; }
	FORCEINLINE void SetWeaponMesh(USkeletalMesh* NewWeaponMesh) { WeaponMesh->SetSkeletalMesh(NewWeaponMesh); }
	FORCEINLINE void SetWeaponParticle(UParticleSystem* NewFireParticle) { FireParticle = NewFireParticle; }
	FORCEINLINE EWeaponState GetWeaponState() const { return WeaponState; }
	FORCEINLINE int32 GetCurrentAmmo() const { return CurrentAmmo; }
	FORCEINLINE int32 GetMaxAmmo() const { return MaxAmmo; }
	FORCEINLINE int32 GetAdditionalAmmo() const { return AdditionalAmmo; }
	FORCEINLINE float GetTimeToReload() const { return TimeToReload; }
	FORCEINLINE void SetAdditionalAmmo(int32 NewValue) { AdditionalAmmo = NewValue; }
	bool CanReload() const;
};
