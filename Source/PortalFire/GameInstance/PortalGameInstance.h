// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "PortalGameInstance.generated.h"

/**
 * 
 */
UCLASS()
class PORTALGAMEENGINESII_API UPortalGameInstance : public UGameInstance
{
	GENERATED_BODY()

private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	int32 MaxNumberPlayers;

public:
	FORCEINLINE void SetMaxNumberPlayers(const int32 MaxPlayers) { MaxNumberPlayers = MaxPlayers; }
	FORCEINLINE int32 GetMaxNumberPlayers() const { return MaxNumberPlayers; }
};
