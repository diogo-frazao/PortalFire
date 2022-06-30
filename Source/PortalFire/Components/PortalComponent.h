// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CheckPortalSpawn.h"
#include "Portal.h"
#include "Components/ActorComponent.h"
#include "PortalComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PORTALGAMEENGINESII_API UPortalComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPortalComponent();

	void SpawnOrangePortal();

protected:

	/** Radius used for checking if can spawn a portal on each spawn point (Top,Bottom,Left,Right) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PortalSpawn", meta = (BlueprintProtected = "true"))
	float PortalCheckSpawnPointsRadius = 30.f;

	/** Class responsible for having the points for checking if a portal can be spawned or not */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PortalPreview", meta = (BlueprintProtected = "true"))
	TSubclassOf<ACheckPortalSpawn> CheckPortalSpawnToSpawn;

	/** Portal Class To spawn */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "PortalSpawn", meta = (BlueprintProtected = "true"))
	TSubclassOf<APortal> PortalToSpawn;

	/** Function to calculate whether can spawn a portal or not */
	UFUNCTION(BlueprintCallable, Category = "PortalPreview")
	bool CheckCanSpawnPortal(TArray<AActor*> ActorsToIgnore, AActor* HitActor, ACheckPortalSpawn* SpawnedPortalPreview, FVector& PortalSpawnPosition, bool bIsPortalBlue);

	/** Function to calculate where to spawn the portal. (Used to adjust portal location) */
	/** Bool used to check whether it was possible to adjust the position or not. If not, don't spawn */
	UFUNCTION(BlueprintCallable, Category = "PortalSpawn")
	bool SetPortalSpawnPosition(bool bHasHitTop, bool bHasHitBottom, bool bHasHitLeft, bool bHasHitRight, ACheckPortalSpawn* SpawnedPortalPreview, FVector& PortalSpawnPosition);

	/** Function to check if another portal is already spawned there
	 * @param bIsPortalBlue If the new portal we want to spawn is blue or yellow
	 * Used because player can replace the same portal but can only spawn a different one if there's enough space
	 */
	UFUNCTION(BlueprintCallable, Category = "PortalSpawn")
	bool GetIsCollidingWithAnotherPortal(TArray<FHitResult> HitResults, bool bIsPortalBlue);

	/** Function to swap the old portal for the new one */
	UFUNCTION(BlueprintCallable, Category = "PortalSpawn")
	void SwapOldPortal(APortal* NewPortal, bool bIsPortalBlue);

	// Portals pointers to know when to swap an old or a newer one
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portals", meta = (BlueprintProtected = "true"))
	APortal* BluePortal;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portals", meta = (BlueprintProtected = "true"))
	APortal* YellowPortal;
	

public:
	/** Spawn Portal on a given point. Called when projectile hits portal wall */
	UFUNCTION(BlueprintCallable, Category = "PortalSpawn")
	void SpawnPortalOnPoint(FVector PointToSpawn, bool bIsPortalBlue);
};
