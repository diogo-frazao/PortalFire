// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalGameMode.h"

#include "GameFramework/GameState.h"
#include "GameFramework/PlayerStart.h"
#include "Kismet/GameplayStatics.h"
#include "PortalGameEnginesII/GameInstance/PortalGameInstance.h"
#include "PortalGameEnginesII/PlayerState/PortalPlayerState.h"

void APortalGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	// Server is the only responsible for starting the game
	if (HasAuthority())
	{
		FTimerHandle PossessPlayersTimerHandle;
		FTimerDelegate PossessPlayersDelegate = FTimerDelegate::CreateUObject(this,
			&APortalGameMode::OnPossessPlayersTimerFinished, NewPlayer);
		GetWorldTimerManager().SetTimer(PossessPlayersTimerHandle, PossessPlayersDelegate,
			0.2f, false);
	}
}

// Called only on server
void APortalGameMode::OnPossessPlayersTimerFinished(APlayerController* NewPlayer)
{
	if (NewPlayer == nullptr) return;
	APortalPlayerController* PlayerController = Cast<APortalPlayerController>(NewPlayer);
    if (PlayerController == nullptr) return;
	
	NumberConnections++;
	
	// If there's at least on client
	if (NumberConnections > 1)
	{
		// Ensure the client possesses a character
		PlayerController->SpawnNewConnectedPlayer();
		bCanUpdatePlayersColors = true;
	}
	// If there's only the host connected, until for more players to be able to play
	else
	{
		APortalGameEnginesIICharacter* Character = Cast<APortalGameEnginesIICharacter>(PlayerController->GetPawn());
		if (Character == nullptr) return;
		Character->SetCanWalk(false);
	}
	
	CheckStartGame();
}

void APortalGameMode::CheckStartGame()
{
	if (PortalGameInstance == nullptr) return;
	GameState = GameState == nullptr ? GetGameState<AGameState>() : GameState; 
	if (GameState == nullptr) return;

	// If all the players are connected
	if (GameState->PlayerArray.Num() == PortalGameInstance->GetMaxNumberPlayers())
	{
		GetWorldTimerManager().SetTimer(StartGameTimerHandle, this, &APortalGameMode::OnGameStartTimerFinished, StartGameDelay);
		if(GEngine)GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow,TEXT("Game will start in 5 seconds."));

	}
	// If there are more players than the maximum accepted for this server
	else if (GameState->PlayerArray.Num() > PortalGameInstance->GetMaxNumberPlayers())
	{
		if (StartGameTimerHandle.IsValid())
		{
			UKismetSystemLibrary::K2_ClearAndInvalidateTimerHandle(this, StartGameTimerHandle);
			if(GEngine)GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red,TEXT("Game start canceled. Server overload."));
		}
	}
}

void APortalGameMode::OnGameStartTimerFinished()
{
	GameState = GameState == nullptr ? GetGameState<AGameState>() : GameState; 
	if (GameState == nullptr) return;

	for (APlayerState* PlayerState : GameState->PlayerArray)
	{
		APortalGameEnginesIICharacter* Character = Cast<APortalGameEnginesIICharacter>(PlayerState->GetPawn());
		if (Character == nullptr || Character->GetController() == nullptr) return;
		RespawnPlayer(Character, Character->GetController());
	}
	if(GEngine)GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Green,TEXT("Game has started!"));
}

void APortalGameMode::BeginPlay()
{
	Super::BeginPlay();
	PrimaryActorTick.bCanEverTick = true;

	// Get game instance after 0.2c. Not available on first frame
	FTimerHandle GameInstanceTimerHandle;
	GetWorld()->GetTimerManager().SetTimer(GameInstanceTimerHandle, this,
		&APortalGameMode::OnGameInstanceTimerFinished, 0.2f);
}

void APortalGameMode::OnGameInstanceTimerFinished()
{
	PortalGameInstance = PortalGameInstance == nullptr ? Cast<UPortalGameInstance>(GetGameInstance()) : PortalGameInstance;
	GameState = GameState == nullptr ? GetGameState<AGameState>() : GameState;
	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStartsAvailable);
}

void APortalGameMode::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (GameState == nullptr) return;

	// If all the players are in the game
	if (GameState->PlayerArray.Num() == NumberConnections && bCanUpdatePlayersColors)
	{
		UE_LOG(LogTemp, Warning, TEXT("Update colors"));
		// Update their colors only once
		FTimerHandle PlayersColorsTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(PlayersColorsTimerHandle,
			this, &APortalGameMode::OnPlayerColorsTimerFinished, 0.2f);
		bCanUpdatePlayersColors = false;
	}
}

void APortalGameMode::OnPlayerColorsTimerFinished()
{
	GameState = GameState == nullptr ? GetGameState<AGameState>() : GameState; 
	if (GameState == nullptr) return;
	
	// Update color for each player connected
	UpdatePlayerBodyColor();
}

// Called only on server by character when health == 0
void APortalGameMode::PlayerEliminated(APortalGameEnginesIICharacter* EliminatedCharacter, APortalPlayerController* EliminatedController, APortalPlayerController* AttackerController)
{
	if (EliminatedCharacter == nullptr) return;

	// Get victim's and attacker's player state to update kills and deaths
	APortalPlayerState* AttackerPlayerState = Cast<APortalPlayerState>(AttackerController->PlayerState);
	if (AttackerPlayerState == nullptr) return;
	APortalPlayerState* EliminatedPlayerState = Cast<APortalPlayerState>(EliminatedController->PlayerState);
	if (EliminatedPlayerState == nullptr) return;

	// Fixes player eliminating itself
	if (AttackerPlayerState == EliminatedPlayerState) return;

	AttackerPlayerState->AddKillAndUpdateHUD(1.f);
	EliminatedPlayerState->AddDeathAndUpdateHUD(1);
	
	EliminatedCharacter->PlayerEliminated();

	UGameplayStatics::GetAllActorsOfClass(this, APlayerStart::StaticClass(), PlayerStartsAvailable);
}

// Called only on server by character when delay to respawn has passed
void APortalGameMode::RespawnPlayer(ACharacter* EliminatedCharacter, AController* EliminatedCharacterController)
{
	// Detach character from controller
	if (EliminatedCharacter == nullptr) return;
	EliminatedCharacter->Reset();
	EliminatedCharacter->Destroy();
	 
	// Respawn character at random player start location
	if (EliminatedCharacterController == nullptr || PlayerStartsAvailable.Num() == 0) return;
	const int32 RandomPlayerStartIndex = FMath::RandRange(0, PlayerStartsAvailable.Num() - 1);
	AActor* PlayerStartToBeSpawnedAt = PlayerStartsAvailable[RandomPlayerStartIndex];
	if (PlayerStartToBeSpawnedAt)
	{
		RestartPlayerAtPlayerStart(EliminatedCharacterController, PlayerStartToBeSpawnedAt);
		PlayerStartsAvailable.Remove(PlayerStartToBeSpawnedAt);
	}
}




