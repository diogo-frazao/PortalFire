// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PortalGameEnginesII/HUD/PortalHUD.h"
#include "PortalPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class PORTALGAMEENGINESII_API APortalPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	void OnPossess(APawn* InPawn) override;

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateHUDHealth(float CurrentHealth, float MaxHealth);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateHUDKills(float CurrentKills);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateHUDDeaths(int32 Deaths);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateHUDCurrentAmmo(int32 Ammo);

	UFUNCTION(BlueprintCallable, Category = "HUD")
	void UpdateHUDAdditionalWeaponAmmo(int32 Ammo);

	UFUNCTION(BlueprintCallable)
	void SpawnNewConnectedPlayer();
	
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<class APortalGameEnginesIICharacter> CharacterClass;
	
private:
	UPROPERTY(BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	APortalHUD* PortalHUD;

public:
	FORCEINLINE APortalHUD* GetPortalHUD() const { return PortalHUD; }
	
};
