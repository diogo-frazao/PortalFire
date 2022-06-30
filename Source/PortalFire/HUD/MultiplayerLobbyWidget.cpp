// Fill out your copyright notice in the Description page of Project Settings.


#include "MultiplayerLobbyWidget.h"

#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
#include "PortalGameEnginesII/GameInstance/PortalGameInstance.h"

bool UMultiplayerLobbyWidget::GetIsValidNumberPlayers() const
{
	const FString NumberPlayersString = NumberPlayersTextBox->GetText().ToString();
	return NumberPlayersString.IsNumeric();
}

void UMultiplayerLobbyWidget::SetMaxPlayersForSession()
{
	UGameInstance* GameInstance = UGameplayStatics::GetGameInstance(this);
	if (GameInstance == nullptr) return;
	UPortalGameInstance* PortalGameInstance = Cast<UPortalGameInstance>(GameInstance);
	if (PortalGameInstance == nullptr) return;

	PortalGameInstance->SetMaxNumberPlayers(GetMaxNumberPlayersForSession());
}

int32 UMultiplayerLobbyWidget::GetMaxNumberPlayersForSession()
{
	const FString NumberPlayersString = NumberPlayersTextBox->GetText().ToString();
	return UKismetStringLibrary::Conv_StringToInt(NumberPlayersString);
}
