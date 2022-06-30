// Fill out your copyright notice in the Description page of Project Settings.


#include "Portal.h"

#include "GameFramework/Actor.h"

#include "PortalGameEnginesIICharacter.h"
#include "PortalGameEnginesIIProjectile.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "Engine/TextureRenderTarget2D.h"
#include "GameFramework/PawnMovementComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
#include "Weapon/Projectile.h"

// Sets default values
APortal::APortal()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	bReplicates = true;

	//////////////////////////////////////////////////////////////////////////
	///Initialize Components
	DefaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	RootComponent = DefaultSceneRoot;

	PortalMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalMesh"));
	PortalMesh->SetupAttachment(RootComponent);
	PortalMesh->SetRelativeRotation(FRotator(-90.f, 180.f, 180.f));

	PortalBorderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("PortalBorderMesh"));
	PortalBorderMesh->SetupAttachment(RootComponent);
	PortalBorderMesh->SetRelativeRotation(FRotator(-90.f, 180.f, 180.f));

	SceneCapture = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("SceneCapture"));
	SceneCapture->SetupAttachment(RootComponent);

	BackFacingScene = CreateDefaultSubobject<USceneComponent>(TEXT("BackFacingScene"));
	BackFacingScene->SetupAttachment(RootComponent);
	BackFacingScene->SetRelativeRotation(FRotator(0.f, 180.f, 0.f));

	PortalCameraQuality = 0.65f;
	bCanTeleportActor = true;
}

void APortal::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(APortal, bIsPortalBlue);
	DOREPLIFETIME(APortal, LinkedPortal);
	DOREPLIFETIME(APortal, ActorInPortal);
}

// Called when the game starts or when spawned
void APortal::BeginPlay()
{
	Super::BeginPlay();
}

void APortal::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	if (!HasAuthority()) return;
	
	// If I haven't overlapped a character or there are no linked portals, don't to anything
	if (OtherActor == nullptr || LinkedPortal == nullptr) {return;}
	
	// If character overlapped this portal, set it to ignore portal walls (can pass through the wall this portal is in)
	ActorInPortal = OtherActor;
	APortalGameEnginesIICharacter* OtherCharacter = Cast<APortalGameEnginesIICharacter>(OtherActor);
	if (OtherCharacter)
	{
		OtherCharacter->GetCapsuleComponent()->SetCollisionProfileName("IgnorePortalWall", true);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("%s"), *OtherActor->GetName());
		AProjectile* OtherProjectile = Cast<AProjectile>(OtherActor);
		if (OtherProjectile == nullptr) {return;}
		OtherProjectile->GetBoxComponent()->SetCollisionProfileName("IgnorePortalWall", true);
		OtherProjectile->SetCanPlayImpactParticles(false);
		UE_LOG(LogTemp, Error, TEXT("Troquei o profile"));
	}
}

void APortal::NotifyActorEndOverlap(AActor* OtherActor)
{
	Super::NotifyActorEndOverlap(OtherActor);

	if (!HasAuthority()) return;

	APortalGameEnginesIICharacter* OtherActorCharacter = Cast<APortalGameEnginesIICharacter>(OtherActor);
	// If I haven't overlapped a character or there are no linked portals, don't to anything
	if (OtherActorCharacter == nullptr || LinkedPortal == nullptr || OtherActorCharacter != ActorInPortal)
	{
		UE_LOG(LogTemp, Warning, TEXT("Portal did not overlapped the character"));
		return;
	}
	// If end overlap this portal, set it to block portal walls again (Pawn by default blocks portal walls)
	ActorInPortal = nullptr;
	OtherActorCharacter->GetCapsuleComponent()->SetCollisionProfileName(TEXT("Pawn"), true);
}

// Called every frame
void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If I'm linked to a portal, adjust linked portal's view 
	if (LinkedPortal != nullptr)
	{
		AdjustLinkedPortalView();

		// If there's a character passing through this portal, check when to teleport it
		if (ActorInPortal != nullptr && HasAuthority())
		{
			CheckActorTeleport();
		}
	}
}

void APortal::AdjustLinkedPortalView()
{
	const FTransform PlayerCameraTransform = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0)
		->GetTransformComponent()->GetComponentTransform();

	// Get the transform of the back facing scene as if it the player camera was a child of it
	// Will store location and rotation relative to back facing scene (used to apply to linked portal)
	const FTransform BackSceneRelativeToCameraTransform = UKismetMathLibrary::MakeRelativeTransform(
		PlayerCameraTransform,GetBackFacingSceneComponent()->GetComponentTransform());

	// Linked portal Scene capture should be at the same distance as the player is from this portal
	// (Because when the player looks at this portal it is seeing the linked portal's view)
	LinkedPortal->GetSceneCaptureComponent()->SetRelativeLocationAndRotation(
		BackSceneRelativeToCameraTransform.GetLocation(),
		BackSceneRelativeToCameraTransform.GetRotation());

	// Make linked portal's scene capture only start to render where the player is
	// Avoids bugs with seeing what's behind of the linked portal
	const float DistanceFromPlayerToThisPortal = UKismetMathLibrary::Vector_Distance(
		PlayerCameraTransform.GetLocation(), GetActorLocation());

	LinkedPortal->GetSceneCaptureComponent()->CustomNearClippingPlane = DistanceFromPlayerToThisPortal + 1.f;

	// Adjust Texture render target to match the screen's resolution
	if (GEngine && GEngine->GameViewport)
	{
		FVector2D ViewportSize;
		GEngine->GameViewport->GetViewportSize(ViewportSize);
		ResizeRenderTarget(GetSceneCaptureComponent()->TextureTarget,
			ViewportSize.X * PortalCameraQuality, ViewportSize.Y * PortalCameraQuality);	
	}
}

// Called only on server
void APortal::CheckActorTeleport()
{
	const auto PlayerVelocity = ActorInPortal->GetVelocity() * GetWorld()->GetDeltaSeconds();
	const FVector PlayerPositionNextFrame = ActorInPortal->GetActorLocation() + PlayerVelocity;

	// Vector pointing from portal to player next frame's position
	FVector PortalToNextPlayerPosition = PlayerPositionNextFrame - GetActorLocation();
	PortalToNextPlayerPosition = PortalToNextPlayerPosition.GetSafeNormal(0.00001f);

	// Player is behind the portal
	if (UKismetMathLibrary::Dot_VectorVector(PortalToNextPlayerPosition, GetActorForwardVector()) < 0.f &&
		bCanTeleportActor)
	{
		TeleportActor();
		
		bCanTeleportActor = false;
		LinkedPortal->bCanTeleportActor = false;
		UE_LOG(LogTemp, Warning, TEXT("No teleport"));

		// Re-enable can player teleport from this portal in 1 second. Avoids teleporting player to same portal
		FTimerHandle EnableCanTeleportPlayer;
		GetWorldTimerManager().SetTimer(EnableCanTeleportPlayer, this,
			&APortal::EnableCanTeleportActor,1.f,false);
	}
}

void APortal::EnableCanTeleportActor()
{
	bCanTeleportActor = true;
	LinkedPortal->bCanTeleportActor = true;
	UE_LOG(LogTemp, Warning, TEXT("Yes teleport"));
}

void APortal::TeleportActor()
{
	FRotator ActorNewRotation;
	// Stored to apply after teleporting the player
	FVector ActorLocalVelocity;

	// Check if actor inside portal is a bullet or a character
	APortalGameEnginesIICharacter* CharacterRef = Cast<APortalGameEnginesIICharacter>(ActorInPortal);
	if (CharacterRef)
	{
		TeleportPlayer(ActorNewRotation, ActorLocalVelocity, CharacterRef);
	}
	else
	{
		AActor* ActorRef = ActorInPortal;
		// Only allow actors with a projectile movement component to be teleported
		if (ActorRef->GetComponentByClass(UProjectileMovementComponent::StaticClass()) == nullptr) return;

		// Get camera direction of player in portal 
		FTransform ActorRelativeDirection = UKismetMathLibrary::MakeRelativeTransform(
			ActorRef->GetTransform(),
			GetBackFacingSceneComponent()->GetComponentTransform());

		// Make it relative to linked portal. Gets Transform of the player when gets out of the linked portal
		FTransform NewActorTransform = UKismetMathLibrary::ComposeTransforms(ActorRelativeDirection,
		                                                                      LinkedPortal->GetActorTransform()); 
		// Store relative player rotation to apply when its teleported
		ActorNewRotation = FRotator(0.f, NewActorTransform.Rotator().Yaw, 0.f);

		// Subtract camera offset. Avoids camera snapping
		FVector NewActorLocationFixed = NewActorTransform.GetLocation();
		
		// Apply new rotation to player
		AProjectile* ActorProjectile = Cast<AProjectile>(ActorRef);
		if (ActorProjectile)
		{
			FActorSpawnParameters SpawnParameters;
			// Set projectile owner and instigator to be this weapon's owner (the character who fired)
			SpawnParameters.Owner = ActorRef->GetOwner();
			SpawnParameters.Instigator = ActorRef->GetInstigator();
			SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			
			AProjectile* NewProjectile = GetWorld()->SpawnActor<AProjectile>(ActorProjectile->ProjectileToSpawnInsidePortal,
				(LinkedPortal->GetActorForwardVector() * 100.f) + NewActorLocationFixed,
				ActorNewRotation, SpawnParameters);

			ActorRef->Destroy();
			ActorInPortal = nullptr;
		}
	}
}

void APortal::TeleportPlayer(FRotator PlayerNewRotation, FVector PlayerLocalVelocity, APortalGameEnginesIICharacter* CharacterRef)
{
	//Get player local velocity
	PlayerLocalVelocity = UKismetMathLibrary::InverseTransformDirection(CharacterRef->GetActorTransform(),
	                                                                    CharacterRef->GetVelocity());

	// Get camera direction of player in portal 
	FTransform PlayerCameraRelativeDirection = UKismetMathLibrary::MakeRelativeTransform(
		CharacterRef->GetFirstPersonCameraComponent()->GetComponentTransform(),
		GetBackFacingSceneComponent()->GetComponentTransform());

	// Make it relative to linked portal. Gets Transform of the player when gets out of the linked portal
	FTransform NewPlayerTransform = UKismetMathLibrary::ComposeTransforms(PlayerCameraRelativeDirection,
	                                                                      LinkedPortal->GetActorTransform()); 

	// Subtract camera offset. Avoids camera snapping
	FVector NewPlayerLocationFixed = NewPlayerTransform.GetLocation() -
		CharacterRef->GetFirstPersonCameraComponent()->GetRelativeLocation();

	// Store relative player rotation to apply when its teleported
	PlayerNewRotation = FRotator(NewPlayerTransform.Rotator().Pitch, NewPlayerTransform.Rotator().Yaw, 0.f);

	// Set Character location in front of linked portal. Avoids getting stuck in wall
	CharacterRef->SetActorLocation((LinkedPortal->GetActorForwardVector() * 150.f) + NewPlayerLocationFixed,
	                               false, nullptr, ETeleportType::TeleportPhysics);
	
	// Apply new rotation to player
	RotateActorInsidePortal(CharacterRef, PlayerNewRotation);
	CharacterRef->GetController()->SetControlRotation(PlayerNewRotation);

	// Convert player local velocity to world (to apply to the player again)
	FTransform PlayerCurrentTransform = UKismetMathLibrary::MakeTransform(
		CharacterRef->GetActorLocation(),
		CharacterRef->GetController()->GetControlRotation(), FVector(1, 1, 1));
	
	FVector PlayerNewWorldVelocity = UKismetMathLibrary::TransformDirection(PlayerCurrentTransform, PlayerLocalVelocity);

	// Apply it to the player
	CharacterRef->GetMovementComponent()->Velocity = PlayerNewWorldVelocity;
}


void APortal::ResizeRenderTarget(UTextureRenderTarget2D* RenderTarget, float SizeX, float SizeY)
{
	if (RenderTarget == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Render Target used to resize texture is nullptr"));
		return;
	}
	RenderTarget->ResizeTarget(SizeX, SizeY);
}

void APortal::LinkPortal(APortal* PortalToLink)
{
	if (PortalToLink == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("Portal to linked passed is nullptr"));
		return;
	}
	LinkedPortal = PortalToLink;
	SetLinkedPortalProperties();
}

void APortal::SetIsPortalBlue(bool Value)
{
	bIsPortalBlue = Value;
	SetPortalProperties();
}

