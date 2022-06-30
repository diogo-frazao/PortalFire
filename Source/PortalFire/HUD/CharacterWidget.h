// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "CharacterWidget.generated.h"

/**
 * 
 */
UCLASS()
class PORTALGAMEENGINESII_API UCharacterWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UProgressBar* HealthBar;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* HealthText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* KillsText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* DeathsText;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* CurrentWeaponAmmo;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	UTextBlock* TotalWeaponAmmo;
};
