// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "Weapon.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "ProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class PORTALGAMEENGINESII_API AProjectileWeapon : public AWeapon
{
	GENERATED_BODY()

public:
	virtual void FireWeapon(const FVector& SpawnLocation, const FRotator& SpawnRotation) override;

protected:
	/** Projectile class to be fired */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fire", meta = (AllowPrivateAccess = "true"))
	TSubclassOf<AActor> ProjectileClass;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Fire", meta = (AllowPrivateAccess = "true"))
	bool bShouldSpendBullet = true;
	
	UPROPERTY()
	AActor* ProjectileCreated = nullptr;
};
