// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "GameFramework/Actor.h"
#include "Ammo.generated.h"

UCLASS()
class PORTALGAMEENGINESII_API AAmmo : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AAmmo();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	
	UPROPERTY(ReplicatedUsing = OnRep_WasPicked, BlueprintReadOnly, Category = "Ammo Properties")
	bool bWasPicked = false;

	UFUNCTION()
	void OnRep_WasPicked();

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> WeaponClassToGiveBulletsTo;

	/** Ammo to give to the player */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo Properties")
	int32 AmmoToAdd = 10;

	/** Ammo's mesh */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo Properties")
	UStaticMeshComponent* AmmoMesh;

	/** Sphere collider where player can pickup ammo */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ammo Properties")
	USphereComponent* AmmoCollectionSphere;

	/** Method called when player overlapped with ammo collection sphere */
	UFUNCTION()
	virtual void OnAmmoOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
