// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "PortalGameEnginesII/PortalGameEnginesIICharacter.h"
#include "PortalPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class PORTALGAMEENGINESII_API APortalPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	virtual void OnRep_Score() override;
	UFUNCTION()
	virtual void OnRep_Deaths();
	
	void AddKillAndUpdateHUD(float Amount);
	void AddDeathAndUpdateHUD(int32 Amount);
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
private:
	UPROPERTY()
	APortalGameEnginesIICharacter* CharacterRef = nullptr;
	UPROPERTY()
	APortalPlayerController* PlayerControllerRef = nullptr;

	UPROPERTY(ReplicatedUsing = OnRep_Deaths, BlueprintReadOnly, Category = "Stats", meta = (AllowPrivateAccess = "true"))
	int32 Deaths;
};
