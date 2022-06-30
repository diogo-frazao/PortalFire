// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CheckPortalSpawn.generated.h"

UCLASS()
class PORTALGAMEENGINESII_API ACheckPortalSpawn : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACheckPortalSpawn();

	FVector GetTopCheckSpawnPointLocation() const;
	FVector GetBottomCheckSpawnPointLocation() const;
	FVector GetRightCheckSpawnPointLocation() const;
	FVector GetLeftCheckSpawnPointLocation() const;
	FVector GetTopRightCheckSpawnPointLocation() const;
	FVector GetTopLeftCheckSpawnPointLocation() const;
	FVector GetBottomRightCheckSpawnPointLocation() const;
	FVector GetBottomLeftCheckSpawnPointLocation() const;

	FVector GetTopSpawnPointLocation() const;
	FVector GetBottomSpawnPointLocation() const;
	
private:

	/** Mesh used to preview where the portal will be spawned. Can be turned off or on */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PortalPreview", meta = (AllowPrivateAccess = "true"))
	USceneComponent* DefaultSceneRoot;
	
	/** Mesh used to preview where the portal will be spawned. Can be turned off or on */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PortalPreview", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* PortalMesh;

	/** Mesh used to preview where the portal's border will be spawned. Can be turned off or on */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PortalPreview", meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* BorderPortalMesh;

	/** Position to use to check if there's enough room BELOW to spawn a portal */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PortalPreview", meta = (AllowPrivateAccess = "true"))
	USceneComponent* BottomCheckSpawnPoint;

	/** Position to use to check if there's enough room ABOVE to spawn a portal */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PortalPreview", meta = (AllowPrivateAccess = "true"))
	USceneComponent* TopCheckSpawnPoint;

	/** Position to use to check if there's enough room on the LEFT to spawn a portal */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PortalPreview", meta = (AllowPrivateAccess = "true"))
	USceneComponent* LeftCheckSpawnPoint;

	/** Position to use to check if there's enough room on the RIGHT to spawn a portal */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PortalPreview", meta = (AllowPrivateAccess = "true"))
	USceneComponent* RightCheckSpawnPoint;

	/** Position to use to spawn the portal if there's no room UP and RIGHT */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PortalPreview", meta = (AllowPrivateAccess = "true"))
	USceneComponent* TopRightCheckSpawnPoint;

	/** Position to use to spawn the portal if there's no room DOWN and RIGHT */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PortalPreview", meta = (AllowPrivateAccess = "true"))
	USceneComponent* BottomRightCheckSpawnPoint;

	/** Position to use to spawn the portal if there's no room UP and LEFT */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PortalPreview", meta = (AllowPrivateAccess = "true"))
	USceneComponent* TopLeftCheckSpawnPoint;

	/** Position to use to spawn the portal if there's no room DOWN and LEFT */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "PortalPreview", meta = (AllowPrivateAccess = "true"))
	USceneComponent* BottomLeftCheckSpawnPoint;
};
