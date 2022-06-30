// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalHUD.h"
#include "Engine/Canvas.h"
#include "Engine/Texture2D.h"
#include "TextureResource.h"
#include "CanvasItem.h"
#include "UObject/ConstructorHelpers.h"

APortalHUD::APortalHUD()
{
	// Set the crosshair texture
	static ConstructorHelpers::FObjectFinder<UTexture2D> CrosshairTexObj(TEXT("/Game/FirstPerson/Textures/FirstPersonCrosshair"));
	CrosshairTex = CrosshairTexObj.Object;
}

void APortalHUD::DrawHUD()
{
	Super::DrawHUD();

	if (!bCanDrawCrosshair) return;

	// Draw very simple crosshair
	// Find center of the Canvas
	const FVector2D Center(Canvas->ClipX * 0.5f, Canvas->ClipY * 0.5f);

	// Offset by half the texture's dimensions so that the center of the texture aligns with the center of the Canvas
	const FVector2D CrosshairDrawPosition( (Center.X),
										   (Center.Y + 20.0f));
	
	// Draw the crosshair
	FCanvasTileItem TileItem( CrosshairDrawPosition, CrosshairTex->Resource, FLinearColor::White);
	TileItem.BlendMode = SE_BLEND_Translucent;
	Canvas->DrawItem( TileItem );
}

void APortalHUD::BeginPlay()
{
	Super::BeginPlay();

	AssignCharacterWidget();
}

// Called locally on every player
void APortalHUD::AssignCharacterWidget()
{
	APlayerController* PlayerController = GetOwningPlayerController();
	if (PlayerController == nullptr || CharacterWidgetClass == nullptr) return;

	CharacterWidget = CreateWidget<UCharacterWidget>(PlayerController, CharacterWidgetClass);
	CharacterWidget->AddToViewport();
}




