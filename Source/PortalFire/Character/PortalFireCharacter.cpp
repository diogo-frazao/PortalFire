// Copyright Epic Games, Inc. All Rights Reserved.

#include "PortalGameEnginesIICharacter.h"
#include "PortalGameEnginesIIProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/InputSettings.h"
#include "Kismet/GameplayStatics.h"
#include "MotionControllerComponent.h"
#include "PortalGameEnginesIIHUD.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameMode/PortalGameMode.h"
#include "Net/UnrealNetwork.h"
#include "PlayerState/PortalPlayerState.h"
#include "Weapon/PortalWeapon.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

//////////////////////////////////////////////////////////////////////////
// APortalGameEnginesIICharacter

APortalGameEnginesIICharacter::APortalGameEnginesIICharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);

	// set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-39.56f, 1.75f, 64.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetRelativeRotation(FRotator(1.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-0.5f, -4.4f, -155.7f));

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(false);			// otherwise won't be visible in the multiplayer
	FP_Gun->bCastDynamicShadow = false;
	FP_Gun->CastShadow = false;
	FP_Gun->SetupAttachment(Mesh1P, TEXT("GripPoint"));
	FP_Gun->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	FP_Gun->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));

	// Create 3rd person gun mesh component
	TpGun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TP_Gun"));
	TpGun->SetOwnerNoSee(true);
	TpGun->SetupAttachment(GetMesh(), TEXT("HandGunSocket"));
	TpGun->SetRelativeLocation(FVector(0.f, 0.f, 0.f));
	TpGun->SetRelativeRotation(FRotator(0.f, 0.f, 0.f));

	FP_MuzzleLocation = CreateDefaultSubobject<USceneComponent>(TEXT("MuzzleLocation"));
	FP_MuzzleLocation->SetupAttachment(FP_Gun);
	FP_MuzzleLocation->SetRelativeLocation(FVector(0.2f, 48.4f, -10.6f));

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 0.0f, 10.0f);

	// Create combat component
	CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));
	CombatComponent->SetIsReplicated(true);
}

void APortalGameEnginesIICharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APortalGameEnginesIICharacter, CurrentHealth);
	DOREPLIFETIME(APortalGameEnginesIICharacter, bCanWalk);
}

void APortalGameEnginesIICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (CombatComponent)
	{
		CombatComponent->Character = this;
	}
}

void APortalGameEnginesIICharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Attach gun mesh component to Skeleton, doing it here because the skeleton is not yet created in the constructor
	FP_Gun->AttachToComponent(Mesh1P, FAttachmentTransformRules(EAttachmentRule::SnapToTarget, true), TEXT("GripPoint"));

	// Show the gun.
	Mesh1P->SetHiddenInGame(false, true);
	
	// Create player controller ref
	PlayerController = PlayerController == nullptr ? Cast<APortalPlayerController>(GetController()) : PlayerController;
	if (PlayerController)
	{
		PlayerController->UpdateHUDHealth(CurrentHealth, MaxHealth);
		PlayerController->UpdateHUDCurrentAmmo(0);
		PlayerController->UpdateHUDAdditionalWeaponAmmo(0);
	}

	// Only receive damage on server
	if (HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &APortalGameEnginesIICharacter::ReceiveDamage);
	}
	
	// Update kills after 4 frames (player state is not available on 1st frame
	GetWorldTimerManager().SetTimer(PlayerStateTimerHandle, this,
		&APortalGameEnginesIICharacter::OnPlayerStateTimerFinished, 0.1);
}

void APortalGameEnginesIICharacter::OnPlayerStateTimerFinished()
{
	APortalPlayerState* MyPlayerState = GetPlayerState<APortalPlayerState>();
	if (MyPlayerState == nullptr) return;
	// Add 0 kills to UI, will just update it without adding any value to the kills
	MyPlayerState->AddKillAndUpdateHUD(0.f);
	MyPlayerState->AddDeathAndUpdateHUD(0);
}

//////////////////////////////////////////////////////////////////////////
// Input

void APortalGameEnginesIICharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);

	// Bind fire portal A event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this,
		&APortalGameEnginesIICharacter::FireButtonPressed);

	// Bind fire portal B event
	PlayerInputComponent->BindAction("FirePortalOrange", IE_Pressed, this,
		&APortalGameEnginesIICharacter::FirePortalOrange);

	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &APortalGameEnginesIICharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &APortalGameEnginesIICharacter::MoveRight);

	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &APortalGameEnginesIICharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &APortalGameEnginesIICharacter::LookUpAtRate);

	PlayerInputComponent->BindAction("NextWeapon", IE_Pressed, this, &APortalGameEnginesIICharacter::ChangeToNextWeapon);
	PlayerInputComponent->BindAction("PreviousWeapon", IE_Pressed, this, &APortalGameEnginesIICharacter::ChangeToPreviousWeapon);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &APortalGameEnginesIICharacter::ReloadButtonPressed);
}

// Generic Fire behavior
APortalGameEnginesIIProjectile* APortalGameEnginesIICharacter::OnFire(USoundBase* FireSound)
{
	// try and fire a projectile
	if (ProjectileClass != nullptr)
	{
		UWorld* const World = GetWorld();
		if (World != nullptr)
		{
			const FRotator SpawnRotation = GetControlRotation();
				
			// MuzzleOffset is in camera space, so transform it to world space before offsetting from the character location to find the final muzzle position
			const FVector SpawnLocation = ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);

			//Set Spawn Collision Handling Override
			FActorSpawnParameters ActorSpawnParams;
			ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

			// try and play the sound if specified
			if (FireSound != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			}

			// try and play a firing animation if specified
			if (FireAnimation != nullptr)
			{
				// Get the animation object for the arms mesh
				UAnimInstance* AnimInstance = Mesh1P->GetAnimInstance();
				if (AnimInstance != nullptr)
				{
					AnimInstance->Montage_Play(FireAnimation, 1.f);
				}
			}
				
			// Spawn the projectile and return it
			return World->SpawnActor<APortalGameEnginesIIProjectile>(ProjectileClass,
				SpawnLocation, SpawnRotation, ActorSpawnParams);
		}
		else return nullptr;
	}
	else return nullptr;
}


void APortalGameEnginesIICharacter::FireButtonPressed()
{
	if (CombatComponent == nullptr || CombatComponent->bIsReloading) return;

	CombatComponent->FireButtonPressed();
}

void APortalGameEnginesIICharacter::ReloadButtonPressed()
{
	if (CombatComponent == nullptr || CombatComponent->EquippedWeapon == nullptr) return;

	CombatComponent->ReloadButtonPressed();
}

// Called when fire portal Orange input is pressed
void APortalGameEnginesIICharacter::FirePortalOrange()
{
	if (CombatComponent == nullptr || CombatComponent->bIsReloading) return;

	CombatComponent->RightFireButtonPressed();
}

void APortalGameEnginesIICharacter::MoveForward(float Value)
{
	if (!bCanWalk) return;
	
	if (CombatComponent == nullptr || CombatComponent->bIsReloading) return;
	
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}

void APortalGameEnginesIICharacter::MoveRight(float Value)
{
	if (!bCanWalk) return;
	
	if (CombatComponent == nullptr || CombatComponent->bIsReloading) return;

	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void APortalGameEnginesIICharacter::TurnAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void APortalGameEnginesIICharacter::LookUpAtRate(float Rate)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

// Called only on server
void APortalGameEnginesIICharacter::EquipWeapon(AWeapon* Weapon)
{
	if (CombatComponent == nullptr || Weapon == nullptr) return;

	// Check if character already has this weapon equipped
	for (const AWeapon* EquippedWeapon : CombatComponent->EquippedWeapons)
	{
		if (EquippedWeapon->GetClass() == Weapon->GetClass())
		{
			return;
		}
	}
	
	if (HasAuthority())
	{
		CombatComponent->AddEquippedWeapon(Weapon);
	}
}

// Server RPC. Called by owing client and executed only on server
void APortalGameEnginesIICharacter::ServerSetWeaponToEquip_Implementation(AWeapon* WeaponToEquip)
{
	if (CombatComponent == nullptr || WeaponToEquip == nullptr) return;

	CombatComponent->SetEquippedWeapon(WeaponToEquip);
}

void APortalGameEnginesIICharacter::ChangeToNextWeapon()
{
	if (!CombatComponent || CombatComponent->EquippedWeapons.Num() <= 1 ||
		CombatComponent->EquippedWeapon == nullptr || CombatComponent->bIsReloading) return;

	const int32 EquippedWeaponIndex = GetEquippedWeaponIndex();
	const int32 NextWeaponIndex = EquippedWeaponIndex == CombatComponent->EquippedWeapons.Num() - 1 ? 0 : EquippedWeaponIndex + 1;
	
	ServerSetWeaponToEquip(CombatComponent->EquippedWeapons[NextWeaponIndex]);
}

void APortalGameEnginesIICharacter::ChangeToPreviousWeapon()
{
	if (!CombatComponent || CombatComponent->EquippedWeapons.Num() <= 1 ||
		CombatComponent->EquippedWeapon == nullptr || CombatComponent->bIsReloading) return;

	const int32 EquippedWeaponIndex = GetEquippedWeaponIndex();
	const int32 PreviousWeaponIndex = EquippedWeaponIndex == 0 ? CombatComponent->EquippedWeapons.Num() - 1 : EquippedWeaponIndex - 1;
	
	ServerSetWeaponToEquip(CombatComponent->EquippedWeapons[PreviousWeaponIndex]);
}

int32 APortalGameEnginesIICharacter::GetEquippedWeaponIndex() const
{
	for (int32 i = 0; i < CombatComponent->EquippedWeapons.Num(); ++i)
	{
		if (CombatComponent->EquippedWeapons[i] == CombatComponent->EquippedWeapon)
		{
			return i;
		}
	}
	return 0;
}

void APortalGameEnginesIICharacter::SetCanWalk(bool Value)
{
	bCanWalk = Value;
}

// Called when this player receives damage
// Called only on server
void APortalGameEnginesIICharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType,
	AController* InstigatorController, AActor* DamageCauser)
{
	CurrentHealth = FMath::Clamp(CurrentHealth - Damage, 0.f, MaxHealth);
	
	// Update character health on server
	PlayerController = PlayerController == nullptr ? Cast<APortalPlayerController>(GetController()) : PlayerController;
	if (PlayerController)
	{
		PlayerController->UpdateHUDHealth(CurrentHealth, MaxHealth);

	}
	// Eliminate player
	if (CurrentHealth == 0.f && bIsAlive)
	{
		APortalGameMode* GameMode = GetWorld()->GetAuthGameMode<APortalGameMode>();
		if (GameMode == nullptr) return;
		APortalPlayerController* AttackerController = Cast<APortalPlayerController>(InstigatorController);
		PlayerController = PlayerController == nullptr ? Cast<APortalPlayerController>(GetController()) : PlayerController;
		if (PlayerController)
		{
			OnPlayerDeath(this);
			GameMode->PlayerEliminated(this, PlayerController, AttackerController);
		}
		bIsAlive = false;
	}
}

// Called on all clients
void APortalGameEnginesIICharacter::OnRep_CurrentHealth()
{
	// Update character health on all clients
	PlayerController = PlayerController == nullptr ? Cast<APortalPlayerController>(GetController()) : PlayerController;
	if (PlayerController)
	{
		PlayerController->UpdateHUDHealth(CurrentHealth, MaxHealth);
	}
}

// Called on Server
void APortalGameEnginesIICharacter::PlayerEliminated()
{
	MulticastPlayerEliminatedEffects(false);

	// Start timer to respawn player
	GetWorldTimerManager().SetTimer(PlayerRespawnTimerHandle, this,
		&APortalGameEnginesIICharacter::OnPlayerRespawnTimerFinished, PlayerRespawnDelay);
}

// Called on server and all clients
void APortalGameEnginesIICharacter::MulticastPlayerEliminatedEffects_Implementation(bool bIsReset)
{	
	if (GetMesh() == nullptr || GetMesh1P() == nullptr) return;

	if (bIsReset)
	{
		// Show gun and hide 3P mesh being dropped
		GetMesh()->SetOwnerNoSee(true);
		GetMesh()->SetSimulatePhysics(false);
		GetMesh1P()->SetVisibility(true, true);
	}
	else
	{
		// Hide gun and show 3P mesh being dropped
		GetMesh()->SetOwnerNoSee(false);
		GetMesh()->SetSimulatePhysics(true);
		GetMesh1P()->SetVisibility(false, true);
	}

	if (!bIsReset)
	{
		// Disable collision on capsule
		GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		PlayerController = PlayerController == nullptr ? Cast<APortalPlayerController>(GetController()) : PlayerController;
		if (PlayerController == nullptr) return;
	
		// If was eliminated, disable movement and input
		GetCharacterMovement()->DisableMovement();
		if (PlayerController) DisableInput(PlayerController);	
	}

	if (bIsReset)
	{
		if (PlayerController == nullptr) return;
		// Only works for player being eliminated. Needs to be last
		if (PlayerController->GetPortalHUD() == nullptr || PlayerController->GetPortalHUD()->CharacterWidget == nullptr) return;
		// Show character HUD
		PlayerController->GetPortalHUD()->CharacterWidget->SetVisibility(ESlateVisibility::Visible);
		PlayerController->GetPortalHUD()->SetCanDrawCrosshair(true);
	}
	else
	{
		if (PlayerController == nullptr) return;
		// Only works for player being eliminated. Needs to be last
		if (PlayerController->GetPortalHUD() == nullptr || PlayerController->GetPortalHUD()->CharacterWidget == nullptr) return;
		// Hide character HUD
		PlayerController->GetPortalHUD()->CharacterWidget->SetVisibility(ESlateVisibility::Hidden);
		PlayerController->GetPortalHUD()->SetCanDrawCrosshair(false);
	}
}

// Called only on server
void APortalGameEnginesIICharacter::OnPlayerRespawnTimerFinished()
{
	// Request character respawn
	APortalGameMode* GameMode = GetWorld()->GetAuthGameMode<APortalGameMode>();
	if (GameMode == nullptr) return;

	// Reset player properties
	MulticastPlayerEliminatedEffects(true);
	
	GameMode->RespawnPlayer(this, Controller);
}

FVector APortalGameEnginesIICharacter::GetFireLocation() const
{
	const FRotator SpawnRotation = GetControlRotation();
	return ((FP_MuzzleLocation != nullptr) ? FP_MuzzleLocation->GetComponentLocation() : GetActorLocation()) + SpawnRotation.RotateVector(GunOffset);
}


