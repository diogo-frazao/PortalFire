// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "RocketProjectile.generated.h"

/**
 * 
 */
UCLASS()
class PORTALGAMEENGINESII_API ARocketProjectile : public AProjectile
{
	GENERATED_BODY()

protected:
	virtual void OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;

	UPROPERTY(EditAnywhere)
	float RocketMinDamage = 10.f;

	UPROPERTY(EditAnywhere)
	float RocketHitInnerRadius = 200.f;

	UPROPERTY(EditAnywhere)
	float RocketHitOuterRadius = 500.f;
};
