// Fill out your copyright notice in the Description page of Project Settings.


#include "CheckPortalSpawn.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
ACheckPortalSpawn::ACheckPortalSpawn()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = DefaultSceneRoot;

	// Create meshes to preview the portal. Rotated to always face the front
	PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalMesh"));
	PortalMesh->SetupAttachment(RootComponent);
	PortalMesh->SetRelativeRotation(FRotator(-90.f, 180.f, 180.f));
	BorderPortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BorderPortalMesh"));
	PortalMesh->SetupAttachment(RootComponent);
	BorderPortalMesh->SetRelativeRotation(FRotator(-90.f, 180.f, 180.f));
	
	BottomCheckSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("BottomCheckSpawn"));
	BottomCheckSpawnPoint->SetupAttachment(BorderPortalMesh);
	BottomCheckSpawnPoint->SetRelativeLocation(FVector(130.f, 0.f, 0.f));

	TopCheckSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("TopCheckSpawn"));
	TopCheckSpawnPoint->SetupAttachment(BorderPortalMesh);
	TopCheckSpawnPoint->SetRelativeLocation(FVector(-130.f, 0.f, 0.f));

	RightCheckSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("RightCheckSpawn"));
	RightCheckSpawnPoint->SetupAttachment(BorderPortalMesh);
	RightCheckSpawnPoint->SetRelativeLocation(FVector(0.f, -100.f, 0.f));

	LeftCheckSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("LeftCheckSpawn"));
	LeftCheckSpawnPoint->SetupAttachment(BorderPortalMesh);
	LeftCheckSpawnPoint->SetRelativeLocation(FVector(0.f, 100.f, 0.f));

	TopRightCheckSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("TopRightCheckSpawn"));
	TopRightCheckSpawnPoint->SetupAttachment(BorderPortalMesh);
	TopRightCheckSpawnPoint->SetRelativeLocation(FVector(-70.f, -90.f, 0.f));

	TopLeftCheckSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("TopLeftCheckSpawn"));
	TopLeftCheckSpawnPoint->SetupAttachment(BorderPortalMesh);
	TopLeftCheckSpawnPoint->SetRelativeLocation(FVector(-70.f, 90.f, 0.f));

	BottomRightCheckSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("BottomRightCheckSpawn"));
	BottomRightCheckSpawnPoint->SetupAttachment(BorderPortalMesh);
	BottomRightCheckSpawnPoint->SetRelativeLocation(FVector(70.f, -90.f, 0.f));

	BottomLeftCheckSpawnPoint = CreateDefaultSubobject<USceneComponent>(TEXT("BottomLeftCheckSpawn"));
	BottomLeftCheckSpawnPoint->SetupAttachment(BorderPortalMesh);
	BottomLeftCheckSpawnPoint->SetRelativeLocation(FVector(70.f, 90.f, 0.f));
}

FVector ACheckPortalSpawn::GetTopCheckSpawnPointLocation() const
{
	return UKismetMathLibrary::TransformLocation(GetActorTransform(), TopCheckSpawnPoint->GetComponentLocation());
}

FVector ACheckPortalSpawn::GetTopSpawnPointLocation() const
{
	return UKismetMathLibrary::TransformLocation(GetActorTransform(),
	TopCheckSpawnPoint->GetComponentLocation() + FVector(0.f,0.f,-50.f));
}

FVector ACheckPortalSpawn::GetBottomCheckSpawnPointLocation() const
{
	return UKismetMathLibrary::TransformLocation(GetActorTransform(), BottomCheckSpawnPoint->GetComponentLocation());
}

FVector ACheckPortalSpawn::GetBottomSpawnPointLocation() const
{
	return UKismetMathLibrary::TransformLocation(GetActorTransform(),
	BottomCheckSpawnPoint->GetComponentLocation()+ FVector(0.f,0.f,40.f));
}

FVector ACheckPortalSpawn::GetRightCheckSpawnPointLocation() const
{
	return UKismetMathLibrary::TransformLocation(GetActorTransform(), RightCheckSpawnPoint->GetComponentLocation());
}

FVector ACheckPortalSpawn::GetLeftCheckSpawnPointLocation() const
{
	return UKismetMathLibrary::TransformLocation(GetActorTransform(), LeftCheckSpawnPoint->GetComponentLocation());
}

FVector ACheckPortalSpawn::GetTopRightCheckSpawnPointLocation() const
{
	return UKismetMathLibrary::TransformLocation(GetActorTransform(), TopRightCheckSpawnPoint->GetComponentLocation());
}

FVector ACheckPortalSpawn::GetTopLeftCheckSpawnPointLocation() const
{
	return UKismetMathLibrary::TransformLocation(GetActorTransform(), TopLeftCheckSpawnPoint->GetComponentLocation());
}

FVector ACheckPortalSpawn::GetBottomRightCheckSpawnPointLocation() const
{
	return UKismetMathLibrary::TransformLocation(GetActorTransform(), BottomRightCheckSpawnPoint->GetComponentLocation());
}

FVector ACheckPortalSpawn::GetBottomLeftCheckSpawnPointLocation() const
{
	return UKismetMathLibrary::TransformLocation(GetActorTransform(), BottomLeftCheckSpawnPoint->GetComponentLocation());
}










