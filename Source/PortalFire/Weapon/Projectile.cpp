// Fill out your copyright notice in the Description page of Project Settings.


#include "Projectile.h"

#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AProjectile::AProjectile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Replicate projectiles from server to clients
	bReplicates = true;
	
	ProjectileCollisionBox = CreateDefaultSubobject<UBoxComponent>(TEXT("CollisionBox"));
	SetRootComponent(ProjectileCollisionBox);
	ProjectileCollisionBox->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	ProjectileCollisionBox->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	// Ignore Physics by default
	ProjectileCollisionBox->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	// Collide exceptions
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_WorldStatic, ECollisionResponse::ECR_Block);
	// Block walls which support portals
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel3, ECollisionResponse::ECR_Block);
	// Block character's mesh
	ProjectileCollisionBox->SetCollisionResponseToChannel(ECollisionChannel::ECC_GameTraceChannel5, ECollisionResponse::ECR_Block);
	
	// Create projectile movement component
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
}

void AProjectile::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AProjectile, bCanPlayImpactParticles);
}

// Called when the game starts or when spawned
void AProjectile::BeginPlay()
{
	Super::BeginPlay();

	// Ignore collision with owner
	ProjectileCollisionBox->IgnoreActorWhenMoving(GetOwner(), true);
	ProjectileCollisionBox->IgnoreActorWhenMoving(this, true);

	// Spawn projectile trace
	if (ProjectileTraceParticle)
	{
		ProjectileTraceComponent = UGameplayStatics::SpawnEmitterAttached(
			ProjectileTraceParticle, ProjectileCollisionBox, FName(),
			GetActorLocation(), GetActorRotation(), EAttachLocation::KeepWorldPosition);
	}

	// Only on server call on hit event
	if (HasAuthority())
	{
		ProjectileCollisionBox->OnComponentHit.AddDynamic(this, &AProjectile::OnProjectileHit);
	}
}

// Called only on server
void AProjectile::OnProjectileHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	// Owner of projectile is the character who fired the weapon with this projectile

	// Check projectile damage if was a headshot
	const float DamageToCause = Hit.BoneName.ToString() == FString("head") ? 100.f : ProjectileDamage;
	
	// Get controller responsible for dealing damage
	const ACharacter* CharacterResponsibleForDamage = Cast<ACharacter>(GetOwner());
	if (CharacterResponsibleForDamage == nullptr) return;
	AController* ControllerResponsibleForDamage = Cast<AController>(CharacterResponsibleForDamage->Controller);
	if (ControllerResponsibleForDamage == nullptr) return;

	// Apply damage to hit character
	UGameplayStatics::ApplyDamage(OtherActor, DamageToCause, ControllerResponsibleForDamage, 
	this, UDamageType::StaticClass());

	if (bCanPlayImpactParticles)
	{
		MulticastPlayImpactParticles();
	}
	Destroy();
}

void AProjectile::MulticastPlayImpactParticles_Implementation()
{
	if (ProjectileHitParticle && bCanPlayImpactParticles)
	{
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ProjectileHitParticle, GetActorTransform());
	}
}

// Replicated automatically (called on server and all clients)
void AProjectile::Destroyed()
{
	Super::Destroyed();
}

// Called every frame
void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}


