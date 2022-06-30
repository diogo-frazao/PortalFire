// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PortalGameEnginesIIProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;

UCLASS(config=Game)
class APortalGameEnginesIIProjectile : public AActor
{
	GENERATED_BODY()
	
public:
	APortalGameEnginesIIProjectile();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category=Projectile)
	USphereComponent* CollisionComp;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovement;

	/** called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Returns CollisionComp subobject **/
	USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	/** Set bshouldSpawnPortalA to value. Called by player depending on portal A or B input */
	UFUNCTION(BlueprintCallable, Category = "PortalSpawn")
	void SetShouldSpawnPortalBlue(bool Value);

protected:
	/** Event used to update Projectile color */
	UFUNCTION(BlueprintImplementableEvent, Category = "DifferentiateProjectiles")
	void UpdateProjectileProperties();
	
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "PortalSpawn", meta = (BlueprintProtected = "true"))
	bool bShouldSpawnPortalBlue;
};

