// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalPlayerState.h"
#include "Net/UnrealNetwork.h"
#include "PortalGameEnginesII/PortalGameEnginesIICharacter.h"

void APortalPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APortalPlayerState, Deaths);
}

// Called only on server
void APortalPlayerState::AddKillAndUpdateHUD(float Amount)
{
	SetScore(GetScore() + Amount);

	CharacterRef = CharacterRef == nullptr ? Cast<APortalGameEnginesIICharacter>(GetPawn()) : CharacterRef;
	if (CharacterRef == nullptr || CharacterRef->GetPlayerController() == nullptr) return;
	PlayerControllerRef = PlayerControllerRef == nullptr ? Cast<APortalPlayerController>(CharacterRef->GetPlayerController()) : PlayerControllerRef;
	if (PlayerControllerRef == nullptr || CharacterRef->GetPlayerController() == nullptr) return;
	
	PlayerControllerRef->UpdateHUDKills(GetScore());
}

// Called only on all clients
void APortalPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	CharacterRef = CharacterRef == nullptr ? Cast<APortalGameEnginesIICharacter>(GetPawn()) : CharacterRef;
	if (CharacterRef == nullptr || CharacterRef->GetPlayerController() == nullptr) return;
	PlayerControllerRef = PlayerControllerRef == nullptr ? Cast<APortalPlayerController>(CharacterRef->GetPlayerController()) : PlayerControllerRef;
	if (PlayerControllerRef == nullptr || CharacterRef->GetPlayerController() == nullptr) return;

	PlayerControllerRef->UpdateHUDKills(GetScore());
}

// Called only on server
void APortalPlayerState::AddDeathAndUpdateHUD(int32 Amount)
{
	Deaths += Amount;

	CharacterRef = CharacterRef == nullptr ? Cast<APortalGameEnginesIICharacter>(GetPawn()) : CharacterRef;
	if (CharacterRef == nullptr || CharacterRef->GetPlayerController() == nullptr) return;
	PlayerControllerRef = PlayerControllerRef == nullptr ? Cast<APortalPlayerController>(CharacterRef->GetPlayerController()) : PlayerControllerRef;
	if (PlayerControllerRef == nullptr || CharacterRef->GetPlayerController() == nullptr) return;
	
	PlayerControllerRef->UpdateHUDDeaths(Deaths);
}

// Called only on all clients
void APortalPlayerState::OnRep_Deaths()
{
	CharacterRef = CharacterRef == nullptr ? Cast<APortalGameEnginesIICharacter>(GetPawn()) : CharacterRef;
	if (CharacterRef == nullptr || CharacterRef->GetPlayerController() == nullptr) return;
	PlayerControllerRef = PlayerControllerRef == nullptr ? Cast<APortalPlayerController>(CharacterRef->GetPlayerController()) : PlayerControllerRef;
	if (PlayerControllerRef == nullptr || CharacterRef->GetPlayerController() == nullptr) return;
	
	PlayerControllerRef->UpdateHUDDeaths(Deaths);
}


