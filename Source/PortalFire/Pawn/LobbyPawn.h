// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "PortalGameEnginesII/HUD/MultiplayerLobbyWidget.h"
#include "LobbyPawn.generated.h"

UCLASS()
class PORTALGAMEENGINESII_API ALobbyPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ALobbyPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UMultiplayerLobbyWidget> LobbyWidgetClass;
};
