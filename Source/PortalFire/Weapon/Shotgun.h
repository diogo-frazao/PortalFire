// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Shotgun.generated.h"

/**
 * 
 */
UCLASS()
class PORTALGAMEENGINESII_API AShotgun : public AWeapon
{
	GENERATED_BODY()

public:
	virtual void FireWeapon(const FVector& MuzzleLocation, const FRotator& SpawnRotation) override;

private:

	FVector GetTraceWithScattered(const FVector& TraceStart, const FVector& TraceEndPoint);
	void CheckTraceHit(const FVector& TraceStart, const FVector& TraceEndPoint, FHitResult& OutHit);
	
	UPROPERTY(EditAnywhere)
	float Damage = 30.f;

	UPROPERTY(EditAnywhere)
	class UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere)
	float FireRange = 2500.f;

	UPROPERTY(EditAnywhere)
	float FireRadius = 75.f;

	UPROPERTY(EditAnywhere)
	int32 NumberOfShots = 5;
};
