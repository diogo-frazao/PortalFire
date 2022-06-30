// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PortalGameEnginesII/Weapon/Weapon.h"
#include "Pickup.generated.h"

UCLASS()
class PORTALGAMEENGINESII_API APickup : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APickup();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	/** Function called when the player overlaps the pickup. */
	/** Used to respawn ammo, weapons, etc */
	virtual void PickupOverlapAction();

	/** Function called when pickup needs to spawn a new pickup class */
	UFUNCTION(BlueprintCallable)
	 virtual void CreateNewSpawnedClass();
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USceneComponent* SceneComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> ClassToSpawn;
	
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	AActor* SpawnedClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* CubeComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	USphereComponent* PickupCollectionSphere;

	UPROPERTY(BlueprintReadOnly)
	bool bIsAlreadyRestartingSpawn = false;

	/**
	 * Pickup Respawn
	 */
	
	virtual void OnRespawnTimerFinished();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pickup", meta = (AllowPrivateAccess = "true"))
	float TimeToRespawn = 3.f;
	
	/** Method called when player overlapped with pickup collection sphere */
	UFUNCTION()
	virtual void OnPickupOverlap(UPrimitiveComponent* OverlappedComponent,AActor* OtherActor,
		UPrimitiveComponent* OtherComp,int32 OtherBodyIndex,bool bFromSweep,const FHitResult& SweepResult);
	
public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
