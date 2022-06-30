// Fill out your copyright notice in the Description page of Project Settings.


#include "PortalComponent.h"
#include "CheckPortalSpawn.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values for this component's properties
UPortalComponent::UPortalComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	
	// Replicate portal component
	SetIsReplicatedByDefault(true);
}

// Called by projectile when it hits a portal wall
void UPortalComponent::SpawnPortalOnPoint(FVector PointToSpawn, bool bIsPortalBlue)
{
	// Used to check if is always colliding with the same actor. Avoids bugs in edges
	TArray<AActor*> ActorsToIgnore;
	
	AActor* HitActor;
	TArray<FHitResult> CenterHitResults;
	// Check if the Point to Spawn is in a wall that supports portal
	bool bCenterTrace = UKismetSystemLibrary::SphereTraceMulti(GetWorld(),
		PointToSpawn, PointToSpawn, PortalCheckSpawnPointsRadius,
		ETraceTypeQuery::TraceTypeQuery3, false, ActorsToIgnore,
		EDrawDebugTrace::None, CenterHitResults,true);

	if (bCenterTrace)
	{
		for (FHitResult HitResult : CenterHitResults)
		{
			UE_LOG(LogTemp, Warning, TEXT("Center: %s"), *HitResult.Actor->GetName());
		}
	}
	
	if (bCenterTrace)
	{
		if (GetIsCollidingWithAnotherPortal(CenterHitResults, bIsPortalBlue)) { return; }
		
		HitActor = CenterHitResults[0].GetActor();
		// If collided with a portal wall, spawn the actor responsible for checking if there's room to spawn a portal.
		FVector PortalSpawnLocation = FVector(CenterHitResults[0].ImpactPoint + (CenterHitResults[0].ImpactNormal * 0.4f));
		ACheckPortalSpawn* SpawnedPortalPreview = GetWorld()->SpawnActor<ACheckPortalSpawn>(CheckPortalSpawnToSpawn,
			PortalSpawnLocation,UKismetMathLibrary::MakeRotFromX(CenterHitResults[0].Normal));
		
		bool bCanSpawnPortal = CheckCanSpawnPortal(ActorsToIgnore, HitActor, SpawnedPortalPreview,
			PortalSpawnLocation, bIsPortalBlue);
		
		if (bCanSpawnPortal)
		{
			APortal* PortalSpawned = GetWorld()->SpawnActor<APortal>(PortalToSpawn, PortalSpawnLocation,
				UKismetMathLibrary::MakeRotFromX(CenterHitResults[0].Normal));
			PortalSpawned->SetIsPortalBlue(bIsPortalBlue);
			
			SwapOldPortal(PortalSpawned, bIsPortalBlue);

			// Link portals
			if (BluePortal != nullptr)
			{
				// Link blue portal to yellow. (If there's no yellow it doesn't do anything)
				BluePortal->LinkPortal(YellowPortal);
			}
			if (YellowPortal != nullptr)
			{
				// Link yellow portal to blue. (If there's no yellow it doesn't do anything)
				YellowPortal->LinkPortal(BluePortal);
			}
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("NO SPAWN"));
		}

		if (SpawnedPortalPreview)
		{
			// Whether a portal was spawned or not, delete the check spawn actor
			SpawnedPortalPreview->Destroy();	
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("No Spawned Portal Preview"));
		}
	}
}

bool UPortalComponent::CheckCanSpawnPortal(TArray<AActor*> ActorsToIgnore, AActor* HitActor,
	ACheckPortalSpawn* SpawnedPortalPreview, FVector& PortalSpawnPosition, bool bIsPortalBlue)
{
	if (SpawnedPortalPreview == nullptr || HitActor == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Spawned portal or hit actor null"));
		return false;
	}

	// Check if there's room above to spawn the portal
	TArray<FHitResult> TopHitResults;
	AActor* TopHitActor = nullptr;
	bool bTopTrace = UKismetSystemLibrary::SphereTraceMulti(GetWorld(),
		SpawnedPortalPreview->GetTopCheckSpawnPointLocation(),
		SpawnedPortalPreview->GetTopCheckSpawnPointLocation(), PortalCheckSpawnPointsRadius,
		ETraceTypeQuery::TraceTypeQuery3, false, ActorsToIgnore,
		EDrawDebugTrace::None, TopHitResults,true);

	if (bTopTrace)
	{
		TopHitActor = TopHitResults[0].GetActor();
		for (FHitResult HitResult : TopHitResults)
		{
			UE_LOG(LogTemp, Warning, TEXT("Top: %s"), *HitResult.Actor->GetName());
		}
	}

	// Check if there's room below to spawn the portal
	TArray<FHitResult> BottomHitResults;
	AActor* BottomHitActor = nullptr;
	bool bBottomTrace = UKismetSystemLibrary::SphereTraceMulti(GetWorld(),
		SpawnedPortalPreview->GetBottomCheckSpawnPointLocation(),
		SpawnedPortalPreview->GetBottomCheckSpawnPointLocation(), PortalCheckSpawnPointsRadius,
		ETraceTypeQuery::TraceTypeQuery3, false, ActorsToIgnore,
		EDrawDebugTrace::None, BottomHitResults,true);

	if (bBottomTrace)
	{
		BottomHitActor = BottomHitResults[0].GetActor();
		for (FHitResult HitResult : BottomHitResults)
		{
			UE_LOG(LogTemp, Warning, TEXT("Bottom: %s"), *HitResult.Actor->GetName());
		}
	}
	
	// Check if there's room on the right to spawn the portal
	TArray<FHitResult> RightHitResults;
	AActor* RightHitActor = nullptr;
	bool bRightTrace = UKismetSystemLibrary::SphereTraceMulti(GetWorld(),
		SpawnedPortalPreview->GetRightCheckSpawnPointLocation(),
		SpawnedPortalPreview->GetRightCheckSpawnPointLocation(), PortalCheckSpawnPointsRadius,
		ETraceTypeQuery::TraceTypeQuery3, false, ActorsToIgnore,
		EDrawDebugTrace::None, RightHitResults,true);

	if (bRightTrace)
	{
		RightHitActor = RightHitResults[0].GetActor();
		for (FHitResult HitResult : RightHitResults)
		{
			UE_LOG(LogTemp, Warning, TEXT("Right: %s"), *HitResult.Actor->GetName());
		}
	}
	
	// Check if there's room above to spawn the portal
	TArray<FHitResult> LeftHitResults;
	AActor* LeftHitActor = nullptr;
	bool bLeftTrace = UKismetSystemLibrary::SphereTraceMulti(GetWorld(),
		SpawnedPortalPreview->GetLeftCheckSpawnPointLocation(),
		SpawnedPortalPreview->GetLeftCheckSpawnPointLocation(), PortalCheckSpawnPointsRadius,
		ETraceTypeQuery::TraceTypeQuery3, false, ActorsToIgnore,
		EDrawDebugTrace::None, LeftHitResults,true);

	if (bLeftTrace)
	{
		LeftHitActor = LeftHitResults[0].GetActor();
		for (FHitResult HitResult : LeftHitResults)
		{
			UE_LOG(LogTemp, Warning, TEXT("Left: %s"), *HitResult.Actor->GetName());
		}
	}
	
	// Check if collided with another portal in any of these traces
	bool bHasHitPortal = GetIsCollidingWithAnotherPortal(TopHitResults, bIsPortalBlue) ||
		GetIsCollidingWithAnotherPortal(BottomHitResults, bIsPortalBlue) ||
			GetIsCollidingWithAnotherPortal(RightHitResults, bIsPortalBlue) ||
			GetIsCollidingWithAnotherPortal(LeftHitResults, bIsPortalBlue);
	if (bHasHitPortal)
	{
		// If will collide with another portal, don't spawn a portal here
		return false;
	}
	
	// Check if it is always the same actor (avoid edges bugs with other actors)
	bool bAllHitTheSameActor = TopHitActor == HitActor && BottomHitActor == HitActor
	&& RightHitActor == HitActor && LeftHitActor == HitActor;
	
	// Check if all of them have hit the wall
	bool bHasHitAll = bTopTrace && bBottomTrace && bRightTrace && bLeftTrace;
	if (!bHasHitAll && bAllHitTheSameActor)
	{
		// If all points haven't hit, adjust it to fit the wall
		// If can't fit to the wall, don't spawn
		return SetPortalSpawnPosition(bTopTrace, bBottomTrace, bLeftTrace, bRightTrace,
			SpawnedPortalPreview, PortalSpawnPosition);
	}

	// Check if it is always the same actor (avoid edges bugs with other actors)
	if (!bAllHitTheSameActor)
	{
		return SetPortalSpawnPosition(TopHitActor == HitActor,
			BottomHitActor == HitActor,LeftHitActor == HitActor,
			RightHitActor == HitActor, SpawnedPortalPreview, PortalSpawnPosition);
	}

	// If all hit the wall and it's always the same actor, just spawn the portal
	return true;
}

bool UPortalComponent::SetPortalSpawnPosition(bool bHasHitTop, bool bHasHitBottom, bool bHasHitLeft,
	bool bHasHitRight, ACheckPortalSpawn* SpawnedPortalPreview, FVector& PortalSpawnPosition)
{
	// Check Spawn Portal if Top trace failed
	if (!bHasHitTop && bHasHitBottom && bHasHitLeft && bHasHitRight)
	{
		UE_LOG(LogTemp, Warning, TEXT("Top Failed, spawned on bottom"));
		PortalSpawnPosition = SpawnedPortalPreview->GetBottomSpawnPointLocation();
		return true;
	}
	
	// Check Spawn Portal if Top and Right trace failed
	if (!bHasHitTop && bHasHitBottom && bHasHitLeft && !bHasHitRight)
	{
		UE_LOG(LogTemp, Warning, TEXT("Top and Right Failed, spawned on bottom left"));
		PortalSpawnPosition = SpawnedPortalPreview->GetBottomLeftCheckSpawnPointLocation();
		return true;
	}

	// Check Spawn Portal if Right trace failed
	if (bHasHitTop && bHasHitBottom && bHasHitLeft && !bHasHitRight)
	{
		UE_LOG(LogTemp, Warning, TEXT("Right Failed, spawned on left"));
		PortalSpawnPosition = SpawnedPortalPreview->GetLeftCheckSpawnPointLocation();
		return true;
	}

	// Check Spawn Portal if Bottom and Right trace failed
	if (bHasHitTop && !bHasHitBottom && bHasHitLeft && !bHasHitRight)
	{
		UE_LOG(LogTemp, Warning, TEXT("Bottom and Right Failed, spawned on Top left"));
		PortalSpawnPosition = SpawnedPortalPreview->GetTopLeftCheckSpawnPointLocation();
		return true;
	}
	
	// Check Spawn Portal if Bottom trace failed
	if (bHasHitTop && !bHasHitBottom && bHasHitLeft && bHasHitRight)
	{
		UE_LOG(LogTemp, Warning, TEXT("Bottom Failed, spawned on Top"));
		PortalSpawnPosition = SpawnedPortalPreview->GetTopSpawnPointLocation();
		return true;
	}

	// Check Spawn Portal if Bottom and Left trace failed
	if (bHasHitTop && !bHasHitBottom && !bHasHitLeft && bHasHitRight)
	{
		UE_LOG(LogTemp, Warning, TEXT("Bottom and Left Failed, spawned on Top Right"));
		PortalSpawnPosition = SpawnedPortalPreview->GetTopRightCheckSpawnPointLocation();
		return true;
	}
	
	// Check Spawn Portal if Left trace failed
	if (bHasHitTop && bHasHitBottom && !bHasHitLeft && bHasHitRight)
	{
		UE_LOG(LogTemp, Warning, TEXT("Left Failed, spawned on Right"));
		PortalSpawnPosition = SpawnedPortalPreview->GetRightCheckSpawnPointLocation();
		return true;
	}
	
	// Check Spawn Portal if Top and Left trace failed
	if (!bHasHitTop && bHasHitBottom && !bHasHitLeft && bHasHitRight)
	{
		UE_LOG(LogTemp, Warning, TEXT("Top and Left Failed, spawned on bottom Right"));
		PortalSpawnPosition = SpawnedPortalPreview->GetBottomRightCheckSpawnPointLocation();
		return true;
	}
	
	// If it wasn't one of these cases, don't spawn the portal
	return false;
}

bool UPortalComponent::GetIsCollidingWithAnotherPortal(TArray<FHitResult> HitResults, bool bIsPortalBlue)
{
	// Avoids crashing or doing unnecessary calculations when trace didn't hit anything
	if (HitResults.Num() == 0) { return false; }

	UE_LOG(LogTemp, Warning, TEXT("%i"), HitResults.Num());
	
	// If collided with at least one portal, check if can't spawn there
	for (FHitResult HitResult : HitResults)
	{
		APortal* HitResultActor = Cast<APortal>(HitResult.Actor);
		
		if (HitResultActor != nullptr)
		{
			// If this is a yellow portal and I'm trying to spawn another, let me move it
			// If are different type of portals, only spawn if there's enough space
			const bool bPortalsAreDifferent = HitResultActor->GetIsPortalBlue() != bIsPortalBlue;
			if (bPortalsAreDifferent)
			{
				return true;
			}
		}
	}
	return false;
}

void UPortalComponent::SwapOldPortal(APortal* NewPortal, bool bIsPortalBlue)
{
	if (bIsPortalBlue)
	{
		// If there's another blue portal, destroy it
		if (BluePortal != nullptr)
		{
			BluePortal->Destroy();
		}
		// Update it to be the newly created
		BluePortal = NewPortal;
	}
	// Is yellow portal
	else
	{
		// If there's another yellow portal, destroy it
		if (YellowPortal != nullptr)
		{
			YellowPortal->Destroy();
		}
		// Update it to be the newly created
		YellowPortal = NewPortal;
	}
}

