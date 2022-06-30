// Fill out your copyright notice in the Description page of Project Settings.

#include "Weapon.h"

#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "PortalGameEnginesII/PortalGameEnginesIICharacter.h"
#include "PortalGameEnginesII/GameMode/PortalGameMode.h"

// Sets default values
AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	// Replicate weapon
	bReplicates = true;

	// Create weapon mesh
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));
	SetRootComponent(WeaponMesh);
	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	// Create weapon collection sphere
	WeaponCollectionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("WeaponCollectionSphere"));
	WeaponCollectionSphere->SetupAttachment(RootComponent);
	// Disable collision by default (only enable on server)
	WeaponCollectionSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	WeaponCollectionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AWeapon, WeaponState);
	DOREPLIFETIME(AWeapon, WeaponColor);
	DOREPLIFETIME(AWeapon, CurrentAmmo);
	DOREPLIFETIME(AWeapon, AdditionalAmmo);
}

// Called when the game starts or when spawned
void AWeapon::BeginPlay()
{
	Super::BeginPlay();

	// Enable sphere collision on server
	if (HasAuthority())
	{
		WeaponCollectionSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		WeaponCollectionSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		WeaponCollectionSphere->OnComponentBeginOverlap.AddDynamic(this, &AWeapon::OnWeaponOverlap);
	}
}

// Called only on server
void AWeapon::OnWeaponOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APortalGameEnginesIICharacter* Character = Cast<APortalGameEnginesIICharacter>(OtherActor);
	if (Character == nullptr) return;
	Character->EquipWeapon(this);
}

// Called every frame
void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Rotate weapon when is on initial state
	if (WeaponState == EWeaponState::EWS_Initial)
	{
		AddActorLocalRotation(FRotator(0.f, 4.f, 0.f));
	}
}

void AWeapon::FireWeapon(const FVector& MuzzleLocation, const FRotator& SpawnRotation)
{
	SpendBullet();
}

void AWeapon::SetAmmoHUD()
{
	// Get character and player controller ref
	CharacterRef = CharacterRef == nullptr ? Cast<APortalGameEnginesIICharacter>(GetOwner()) : CharacterRef;
	if (CharacterRef == nullptr) return;
	ControllerRef = ControllerRef == nullptr ? Cast<APortalPlayerController>(CharacterRef->Controller) : ControllerRef;
	if (ControllerRef == nullptr) return;

	// Update current ammo HUD
	ControllerRef->UpdateHUDCurrentAmmo(CurrentAmmo);
	// Update additional ammo HUD
	ControllerRef->UpdateHUDAdditionalWeaponAmmo(AdditionalAmmo);
}

// Called only on server
void AWeapon::SpendBullet()
{
	CurrentAmmo = FMath::Clamp(CurrentAmmo - 1, 0, MaxAmmo);
	SetAmmoHUD();
}

bool AWeapon::CanReload() const
{
	return CurrentAmmo < MaxAmmo && AdditionalAmmo > 0;
}

// Called on all clients
void AWeapon::OnRep_CurrentAmmo()
{
	SetAmmoHUD();
}

// Called only on all clients
// Fixes gun not updating HUD sometimes due to latency
void AWeapon::OnRep_Owner()
{
	Super::OnRep_Owner();
	SetAmmoHUD();
}

// Called on all clients
void AWeapon::OnRep_AdditionalAmmo()
{
	SetAmmoHUD();
}

// Called only on server
void AWeapon::SetWeaponState(EWeaponState NewState)
{
	WeaponState = NewState;

	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		WeaponCollectionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}
}

// Called only on all clients
void AWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		WeaponCollectionSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	}
}

// Called only on server
void AWeapon::AddCurrentAmmoAndUpdateHUD(int32 Amount)
{
	CurrentAmmo = FMath::Clamp(CurrentAmmo + Amount, 0, MaxAmmo);
	SetAmmoHUD();
}

// Called only on server
void AWeapon::AddAdditionalAmmoAndUpdateHUD(int32 Amount)
{
	AdditionalAmmo += Amount;
	SetAmmoHUD();
}





