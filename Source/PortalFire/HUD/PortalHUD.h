// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CharacterWidget.h"
#include "GameFramework/HUD.h"
#include "PortalHUD.generated.h"

/**
 * 
 */
UCLASS()
class PORTALGAMEENGINESII_API APortalHUD : public AHUD
{
	GENERATED_BODY()
public:
	APortalHUD();

	/** Assigned in BP. Corresponds to HUD BP for the characters to use*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Stats")
	TSubclassOf<UUserWidget> CharacterWidgetClass;
	
	/** Variable used to access the character widget which contains the HUD */
	UPROPERTY(BlueprintReadOnly)
	UCharacterWidget* CharacterWidget;
	
	/** Primary draw call for the HUD */
	virtual void DrawHUD() override;

protected:
	virtual void BeginPlay() override;
	void AssignCharacterWidget(); 

private:
	/** Crosshair asset pointer */
	class UTexture2D* CrosshairTex;

	UPROPERTY(BlueprintReadWrite, meta = (AllowPrivateAccess = true))
	bool bCanDrawCrosshair = true;

public:
	FORCEINLINE void SetCanDrawCrosshair(bool Value) { bCanDrawCrosshair = Value; }
};
