// Fill out your copyright notice in the Description page of Project Settings.


#include "RocketProjectile.h"

#include "Kismet/GameplayStatics.h"


// Called only on server
void ARocketProjectile::OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor,
                                        UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	// Get character who fired the projectile
	APawn* PawnWhoFiredWeapon = GetInstigator();
	if (PawnWhoFiredWeapon == nullptr) return;
	AController* ControllerWhoFiredWeapon = PawnWhoFiredWeapon->GetController();
	if (ControllerWhoFiredWeapon == nullptr) return;

	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(PawnWhoFiredWeapon);
	
	UGameplayStatics::ApplyRadialDamageWithFalloff(this, ProjectileDamage, RocketMinDamage,
		GetActorLocation(), RocketHitInnerRadius, RocketHitOuterRadius, 1.f, UDamageType::StaticClass(),
		ActorsToIgnore, this, ControllerWhoFiredWeapon);
	
	Super::OnProjectileHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}
