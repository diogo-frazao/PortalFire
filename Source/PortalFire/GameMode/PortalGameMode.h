// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "PortalGameEnginesII/PortalGameEnginesIICharacter.h"
#include "PortalGameEnginesII/Weapon/Projectile.h"
#include "PortalGameMode.generated.h"

/**
 * 
 */
UCLASS()
class PORTALGAMEENGINESII_API APortalGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	virtual void Tick(float DeltaSeconds) override;
	virtual void PostLogin(APlayerController* NewPlayer) override;

	/** Function called when a player is eliminated */
	UFUNCTION(BlueprintCallable, Category = "Eliminations")
	void PlayerEliminated(APortalGameEnginesIICharacter* EliminatedCharacter, APortalPlayerController* EliminatedController, APortalPlayerController* AttackerController);

	/** Function called to respawn the player */
	UFUNCTION(BlueprintCallable, Category = "Eliminations")
	void RespawnPlayer(ACharacter* EliminatedCharacter, AController* EliminatedCharacterController);

	/** Checks the conditions to start the game */
	UFUNCTION(BlueprintCallable)
	void CheckStartGame();

protected:
	virtual void BeginPlay() override;

	/** The number of connections on this particular server */
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 NumberConnections = 0;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bCanUpdatePlayersColors = true;

	UFUNCTION(BlueprintImplementableEvent)
	void UpdatePlayerBodyColor();

private:
	class UPortalGameInstance* PortalGameInstance = nullptr;
	class AGameState* GameState = nullptr;
	void OnGameInstanceTimerFinished();
	void OnPlayerColorsTimerFinished();
	void OnPossessPlayersTimerFinished(APlayerController* NewPlayer);

	FTimerHandle StartGameTimerHandle;

	/** Function called to start the game. Will respawn all the players */
	UFUNCTION(BlueprintCallable)
	void OnGameStartTimerFinished();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	float StartGameDelay = 5.f;

	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<AActor*> PlayerStartsAvailable;
};
