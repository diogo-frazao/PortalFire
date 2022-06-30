// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalPlayerController.h"

#include <string>

#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "PortalGameEnginesII/PortalGameEnginesIICharacter.h"

void APortalPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// Create local reference for HUD on this player
	PortalHUD = Cast<APortalHUD>(GetHUD());
}

void APortalPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	const APortalGameEnginesIICharacter* PortalCharacter = Cast<APortalGameEnginesIICharacter>(InPawn);
	if (PortalCharacter == nullptr) return;

	UpdateHUDHealth(PortalCharacter->GetCurrentHealth(), PortalCharacter->GetMaxHealth());
}

void APortalPlayerController::UpdateHUDHealth(float CurrentHealth, float MaxHealth)
{
	PortalHUD = PortalHUD == nullptr ? Cast<APortalHUD>(GetHUD()) : PortalHUD;
	if (PortalHUD == nullptr || PortalHUD->CharacterWidget == nullptr ||
		PortalHUD->CharacterWidget->HealthBar == nullptr ||
		PortalHUD->CharacterWidget->HealthText == nullptr) return;

	// Update health bar
	const float HealthPercent = CurrentHealth / MaxHealth;
	PortalHUD->CharacterWidget->HealthBar->SetPercent(HealthPercent);
	// Update health text
	const FString HealthString = FString::FromInt(FMath::CeilToInt(CurrentHealth));
	const FText HealthText = FText::FromString(HealthString);
	PortalHUD->CharacterWidget->HealthText->SetText(HealthText);
}

void APortalPlayerController::UpdateHUDKills(float CurrentKills)
{
	PortalHUD = PortalHUD == nullptr ? Cast<APortalHUD>(GetHUD()) : PortalHUD;
	if (PortalHUD == nullptr || PortalHUD->CharacterWidget == nullptr ||
		PortalHUD->CharacterWidget->KillsText == nullptr) return;

	// Update kills text
	const FString KillsString = FString::FromInt(FMath::CeilToInt(CurrentKills));
	const FText KillsText = FText::FromString(KillsString);
	PortalHUD->CharacterWidget->KillsText->SetText(KillsText);
}

void APortalPlayerController::UpdateHUDDeaths(int32 Deaths)
{
	PortalHUD = PortalHUD == nullptr ? Cast<APortalHUD>(GetHUD()) : PortalHUD;
	if (PortalHUD == nullptr || PortalHUD->CharacterWidget == nullptr ||
		PortalHUD->CharacterWidget->DeathsText == nullptr) return;

	// Update deaths text
	const FString DeathsString = FString::FromInt(Deaths);
	const FText DeathsText = FText::FromString(DeathsString);
	PortalHUD->CharacterWidget->DeathsText->SetText(DeathsText);
}

void APortalPlayerController::UpdateHUDCurrentAmmo(int32 Ammo)
{
	PortalHUD = PortalHUD == nullptr ? Cast<APortalHUD>(GetHUD()) : PortalHUD;
	if (PortalHUD == nullptr || PortalHUD->CharacterWidget == nullptr ||
		PortalHUD->CharacterWidget->CurrentWeaponAmmo == nullptr) return;

	// Update current weapon ammo text
	const FString CurrentAmmoString = FString::FromInt(Ammo);
	const FText CurrentAmmoText = FText::FromString(CurrentAmmoString);
	PortalHUD->CharacterWidget->CurrentWeaponAmmo->SetText(CurrentAmmoText);
}

void APortalPlayerController::UpdateHUDAdditionalWeaponAmmo(int32 Ammo)
{
	PortalHUD = PortalHUD == nullptr ? Cast<APortalHUD>(GetHUD()) : PortalHUD;
	if (PortalHUD == nullptr || PortalHUD->CharacterWidget == nullptr ||
		PortalHUD->CharacterWidget->TotalWeaponAmmo == nullptr) return;

	// Update total weapon ammo text
	const FString TotalAmmoString = FString::FromInt(Ammo);
	const FText TotalAmmoText = FText::FromString(TotalAmmoString);
	PortalHUD->CharacterWidget->TotalWeaponAmmo->SetText(TotalAmmoText);
}

// Called by gameplay game mode to ensure all clients possess a character
void APortalPlayerController::SpawnNewConnectedPlayer()
{
	// Create new character and possess it
	if (GetPawn() == nullptr)
	{
		// Get random player start location
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStarts);
		const int32 RandomPlayerStartIndex = FMath::RandRange(0, PlayerStarts.Num() - 1);

		// Spawn a new character on a random player start
		FActorSpawnParameters CharacterSpawnParameters;
		CharacterSpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
		APortalGameEnginesIICharacter* SpawnedCharacter = GetWorld()->SpawnActor<APortalGameEnginesIICharacter>(
			CharacterClass,PlayerStarts[RandomPlayerStartIndex]->GetActorTransform(),
			CharacterSpawnParameters);

		// Possess character and disable walk while all players are not ready
		if (SpawnedCharacter == nullptr) return;
		Possess(SpawnedCharacter);
		SpawnedCharacter->SetCanWalk(false);
	}
	// Disable walk while all players are not ready
	else
	{
		APortalGameEnginesIICharacter* PortalCharacter = Cast<APortalGameEnginesIICharacter>(GetPawn());
		if (PortalCharacter == nullptr) return;
		PortalCharacter->SetCanWalk(false);
	}
}
