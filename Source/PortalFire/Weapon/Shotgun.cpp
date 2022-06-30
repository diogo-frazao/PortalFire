// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"

#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "PortalGameEnginesII/PortalGameEnginesIICharacter.h"

void AShotgun::FireWeapon(const FVector& MuzzleLocation, const FRotator& SpawnRotation)
{
	Super::FireWeapon(MuzzleLocation, SpawnRotation);
	
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;
	AController* InstigatorController = OwnerPawn->GetController();

	// For each bullet spread from the shotgun
	for (int32 i = 0; i < NumberOfShots; i++)
	{
		// Calculate start and final trace points
		const FVector FinalTracePoint = UKismetMathLibrary::GetForwardVector(SpawnRotation) * FireRange;
		FVector TraceEnd = MuzzleLocation + FinalTracePoint;
		FVector FireTraceEnd = GetTraceWithScattered(MuzzleLocation, TraceEnd);
		
		// Check if hit something
		FHitResult FireHitResult;
		CheckTraceHit(MuzzleLocation, FireTraceEnd, FireHitResult);

		// Check if hit a character
		APortalGameEnginesIICharacter* OtherCharacter = Cast<APortalGameEnginesIICharacter>(FireHitResult.GetActor());

		// Only apply damage on server
		if (OtherCharacter && HasAuthority() && InstigatorController)
		{
			UGameplayStatics::ApplyDamage(OtherCharacter, Damage, InstigatorController,
		this, UDamageType::StaticClass());
		}
	}
}

void AShotgun::CheckTraceHit(const FVector& TraceStart, const FVector& TraceEndPoint, FHitResult& OutHit)
{
	// Line trace from shots ignoring actor who shot the weapon
	GetWorld()->LineTraceSingleByChannel(OutHit, TraceStart, TraceEndPoint, ECollisionChannel::ECC_Visibility);

	FVector ShotEnd = TraceEndPoint;
	if (OutHit.bBlockingHit)
	{
		ShotEnd = OutHit.ImpactPoint;
	}
	
	// Spawn impact particles
	if (ImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, OutHit.ImpactPoint,
			OutHit.ImpactNormal.Rotation());
	}
}

FVector AShotgun::GetTraceWithScattered(const FVector& TraceStart, const FVector& TraceEndPoint)
{
	const FVector ToEndNormalized = (TraceEndPoint - TraceStart).GetSafeNormal();
	const FVector SphereFinalPoint = TraceStart + ToEndNormalized * FireRange;
	
	// Get scattered point
	const FVector RandomScatteredWorldPoint = UKismetMathLibrary::RandomUnitVector() *
		FMath::FRandRange(0.f, FireRadius);
	const FVector RandomScatteredSpherePoint = SphereFinalPoint + RandomScatteredWorldPoint;
	FVector FromStartToRandomSpherePoint = RandomScatteredSpherePoint - TraceStart;

	return FVector(TraceStart + FromStartToRandomSpherePoint * 20.f);
}

