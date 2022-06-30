// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Projectile.generated.h"

UCLASS()
class PORTALGAMEENGINESII_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AProjectile();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
	// Called every frame
    virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;

	UPROPERTY(EditAnywhere)
	TSubclassOf<AProjectile> ProjectileToSpawnInsidePortal;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Projectile damage when hits a character */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Projectile Collision", meta = (AllowPrivateAccess = "true"))
	float ProjectileDamage = 20.f;

	UPROPERTY(Replicated, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bCanPlayImpactParticles = true;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastPlayImpactParticles();

private:

	/** Collision box used to detect collision on the projectile */
	UPROPERTY(EditAnywhere, Category = "Projectile Collision")
	UBoxComponent* ProjectileCollisionBox;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UProjectileMovementComponent* ProjectileMovementComponent;

	/** Trace particle that follows the projectile */
	UPROPERTY(EditAnywhere, BlueprintReadOnly,  meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ProjectileTraceParticle;

	/** Impact particle to be played when projectile hits something */
	UPROPERTY(EditAnywhere, BlueprintReadOnly,  meta = (AllowPrivateAccess = "true"))
	UParticleSystem* ProjectileHitParticle;
	
	UParticleSystemComponent* ProjectileTraceComponent;
	
public:	
	FORCEINLINE UBoxComponent* GetBoxComponent() const { return ProjectileCollisionBox; }
	FORCEINLINE void SetCanPlayImpactParticles(bool Value) { bCanPlayImpactParticles = Value; }
};
