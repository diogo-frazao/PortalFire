// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PortalGameEnginesIICharacter.h"
#include "GameFramework/Actor.h"
#include "Portal.generated.h"

UCLASS()
class PORTALGAMEENGINESII_API APortal : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	APortal();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/** Returns the scene capture component of the portal */
	FORCEINLINE USceneCaptureComponent2D* GetSceneCaptureComponent() const { return SceneCapture; }

	/** Returns the back facing scene of the portal */
	FORCEINLINE USceneComponent* GetBackFacingSceneComponent() const { return BackFacingScene; }

	/** Returns the value of bIsPortalBlue. Used to know if this portal is blue or yellow */
	FORCEINLINE bool GetIsPortalBlue() const { return bIsPortalBlue; }

	/** Property used to choose the quality of the portal's camera. 1 Means screen resolution, 0.5 half, etc.
	 * Lower to increase performance */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "SceneCapture")
	float PortalCameraQuality;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	/** Event called to update portal's properties whether it is blue or yellow */
	UFUNCTION(BlueprintImplementableEvent, Category = "DifferentiatePortals")
	void SetPortalProperties();

	/** Event called when this portal is linked to another. Material of this portal is the one it is connected to */
	UFUNCTION(BlueprintImplementableEvent, Category = "Portal")
	void SetLinkedPortalProperties();

	/** Function to resize the scene capture to match the screen's size */
	UFUNCTION(BlueprintCallable, Category = "SceneCapture", meta = (BlueprintProtected = "true"))
	void ResizeRenderTarget(UTextureRenderTarget2D* RenderTarget, float SizeX, float SizeY);

	// Portal I am linked to
	UPROPERTY(Replicated, VisibleDefaultsOnly, BlueprintReadOnly, Category = "Portal", meta = (BlueprintProtected = "true"))
	APortal* LinkedPortal;

	/** Begin Overlap Event */
	UFUNCTION()
	virtual void NotifyActorBeginOverlap(AActor* OtherActor) override;

	/** End Overlap Event */
	UFUNCTION()
	virtual void NotifyActorEndOverlap(AActor* OtherActor) override;

	// Character that is inside this portal
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "Portal", meta = (BlueprintProtected = "true"))
	AActor* ActorInPortal;
	
	UFUNCTION(BlueprintImplementableEvent)
	void RotateActorInsidePortal(AActor* ActorRef, FRotator NewRotation);
	
	/** Adjusts the linked portal scene capture to match the camera's view and player distance */
	UFUNCTION(BlueprintCallable, Category = "SceneCapture", meta = (BlueprintProtected = "true"))
	void AdjustLinkedPortalView();

	/** Checks if the player is behind the portal. If it is, teleport him */
	UFUNCTION(BlueprintCallable, Category = "SceneCapture", meta = (BlueprintProtected = "true"))
	void CheckActorTeleport();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal", meta = (BlueprintProtected = "true"))
	bool bCanTeleportActor;

	UFUNCTION(BlueprintCallable, Category = "Portal", meta = (BlueprintProtected = "true"))
	void EnableCanTeleportActor();
	
	void TeleportPlayer(FRotator PlayerNewRotation, FVector PlayerLocalVelocity,
	                    APortalGameEnginesIICharacter* CharacterRef);

	UFUNCTION(BlueprintCallable, Category = "Teleport", meta = (BlueprintProtected = "true"))
	void TeleportActor();

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	/** Function to differentiate whether the spawned portal is portal blue or yellow */
	UFUNCTION(BlueprintCallable, Category = "DifferentiatePortals")
	void SetIsPortalBlue(bool Value);

	/** Links this portal to the one passed as parameter */
	UFUNCTION(BlueprintCallable, Category = "Portal")
	void LinkPortal(APortal* PortalToLink);

private:

	/** Default scene root */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	USceneComponent* DefaultSceneRoot;
		
	/** Portal Mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* PortalMesh;

	/** Portal Border Mesh */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* PortalBorderMesh;

	/** Scene capture used to capture the other portal's view */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	USceneCaptureComponent2D* SceneCapture;

	/** Scene component pointing backwards. (Used to look from one portal to the other) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Portal", meta = (AllowPrivateAccess = "true"))
	USceneComponent* BackFacingScene;

	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly, Category = "DifferentiatePortals", meta = (AllowPrivateAccess = "true"))
	bool bIsPortalBlue;
};
