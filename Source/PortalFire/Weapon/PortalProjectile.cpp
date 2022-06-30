// Copyright Epic Games, Inc. All Rights Reserved.

#include "PortalGameEnginesIIProjectile.h"
#include "PortalComponent.h"
#include "PortalGameEnginesIICharacter.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "Net/UnrealNetwork.h"

APortalGameEnginesIIProjectile::APortalGameEnginesIIProjectile() 
{
	// Use a sphere as a simple collision representation
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(5.0f);
	CollisionComp->BodyInstance.SetCollisionProfileName("Projectile");
	CollisionComp->OnComponentHit.AddDynamic(this, &APortalGameEnginesIIProjectile::OnHit);		// set up a notification for when this component hits something blocking

	// Players can't walk on it
	CollisionComp->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionComp->CanCharacterStepUpOn = ECB_No;

	// Set as root component
	RootComponent = CollisionComp;

	// Use a ProjectileMovementComponent to govern this projectile's movement
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 3000.f;
	ProjectileMovement->MaxSpeed = 3000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->bShouldBounce = true;

	// Die after 3 seconds by default
	InitialLifeSpan = 3.0f;
}

void APortalGameEnginesIIProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(APortalGameEnginesIIProjectile, bShouldSpawnPortalBlue);
}


void APortalGameEnginesIIProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if ((OtherActor != nullptr) && (OtherActor != this) && HasAuthority())
	{
		// If collided with portal wall, try to spawn projectile
		if (OtherComp->GetCollisionObjectType() == ECollisionChannel::ECC_GameTraceChannel2)
		{
			//UE_LOG(LogTemp, Warning, TEXT("Collided with portal wall"));
		
			UPortalComponent* CharacterPortalComponent = GetOwner()->FindComponentByClass<UPortalComponent>();
			if (CharacterPortalComponent == nullptr)
			{
				UE_LOG(LogTemp, Warning, TEXT("Character doesn't have Portal Component"));
				return;
			}
			CharacterPortalComponent->SpawnPortalOnPoint(Hit.ImpactPoint, bShouldSpawnPortalBlue);
		}
		// If is not a portal wall, don't spawn anything and destroy self
		Destroy();
	}
}

void APortalGameEnginesIIProjectile::SetShouldSpawnPortalBlue(bool Value)
{
	bShouldSpawnPortalBlue = Value;
	UpdateProjectileProperties();
}
