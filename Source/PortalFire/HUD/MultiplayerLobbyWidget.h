// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableTextBox.h"
#include "MultiplayerLobbyWidget.generated.h"

/**
 * 
 */
UCLASS()
class PORTALGAMEENGINESII_API UMultiplayerLobbyWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(meta = (BindWidget))
	UEditableTextBox* NumberPlayersTextBox;

	UFUNCTION(BlueprintPure)
	bool GetIsValidNumberPlayers() const;

	UFUNCTION(BlueprintCallable)
	int32 GetMaxNumberPlayersForSession();

	/** Used to set the max players for this server's session */
	UFUNCTION(BlueprintCallable)
	void SetMaxPlayersForSession();

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void HostServer();

	/** What to do when player cannot host server or connect to existing session */
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable)
	void OnConnectionFailed();
};
