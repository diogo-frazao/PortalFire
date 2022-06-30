// Fill out your copyright notice in the Description page of Project Settings.


#include "Pickup.h"

#include "PortalGameEnginesII/PortalGameEnginesIICharacter.h"

// Sets default values
APickup::APickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	// Create scene root
	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("RootSceneComponent"));
	SetRootComponent(SceneComponent);

	// Create cube component (only for visuals)
	CubeComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CubeComponent"));
	CubeComponent->SetupAttachment(RootComponent);
	CubeComponent->SetRelativeLocation(FVector(0.f, 0.f, -20.f));
	CubeComponent->SetWorldScale3D(FVector(0.5f, 0.5f, 0.5f));

	// Create pickup collection sphere
	PickupCollectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("PickupCollectionSphere"));
	PickupCollectionSphere->SetupAttachment(RootComponent);
	// Disable collision by default (only enable on server)
	PickupCollectionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	PickupCollectionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void APickup::BeginPlay()
{
	Super::BeginPlay();

	if (ClassToSpawn == nullptr) return;

	if (HasAuthority())
	{
		CreateNewSpawnedClass();
	}
}

// Called only on server
void APickup::CreateNewSpawnedClass()
{
	// Instantiate pickup to collect only on server
	const FVector WeaponSpawnLocation(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 50.f);
	SpawnedClass = GetWorld()->SpawnActor<AActor>(ClassToSpawn, WeaponSpawnLocation,
												  FRotator(0.f,0.f,0.f));

	// Enable pickup collection only on server
	PickupCollectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	PickupCollectionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
	PickupCollectionSphere->OnComponentBeginOverlap.AddDynamic(this, &APickup::OnPickupOverlap);
}

// Called only on server. Implemented on child classes
void APickup::OnPickupOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                              UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
}

// Called only on server. Implemented on child classes
void APickup::PickupOverlapAction()
{
}

// Called only on server. Implemented on child classes
void APickup::OnRespawnTimerFinished()
{
}

// Called every frame
void APickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

