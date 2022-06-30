// Fill out your copyright notice in the Description page of Project Settings.


#include "Ammo.h"

#include "Net/UnrealNetwork.h"
#include "PortalGameEnginesII/PortalGameEnginesIICharacter.h"

// Sets default values
AAmmo::AAmmo()
{
	PrimaryActorTick.bCanEverTick = true;

	// Replicate ammo
	bReplicates = true;

	// Create ammo mesh
	AmmoMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("AmmoMesh"));
	SetRootComponent(AmmoMesh);
	AmmoMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AmmoMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// Create ammo collection sphere
	AmmoCollectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AmmoCollectionSphere"));
	AmmoCollectionSphere->SetupAttachment(RootComponent);
	// Disable collision by default (only enable on server)
	AmmoCollectionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AmmoCollectionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AAmmo::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AAmmo, bWasPicked);
}

// Called when the game starts or when spawned
void AAmmo::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		AmmoCollectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AmmoCollectionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		AmmoCollectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AAmmo::OnAmmoOverlap);
	}
}

// Called every frame
void AAmmo::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Rotate weapon when is on initial state
	if (!bWasPicked)
	{
		AddActorLocalRotation(FRotator(0.f, 4.f, 0.f));
	}
}

// Called only on server when player overlaps ammo collection sphere
void AAmmo::OnAmmoOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APortalGameEnginesIICharacter* Character = Cast<APortalGameEnginesIICharacter>(OtherActor);
	if (Character == nullptr || Character->GetCombatComponent() == nullptr ||
		Character->GetCombatComponent()->GetEquippedWeapon() == nullptr) return;

	// If equipped weapon is of the correct ammo type
	if (Character->GetCombatComponent()->GetEquippedWeapon()->GetClass() == WeaponClassToGiveBulletsTo)
	{
		Character->GetCombatComponent()->AddAmmoToEquippedWeapon(AmmoToAdd);
		
		bWasPicked = true;
		AmmoCollectionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetActorHiddenInGame(true);
	}
}

// Called only on all clients
void AAmmo::OnRep_WasPicked()
{
	AmmoCollectionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

