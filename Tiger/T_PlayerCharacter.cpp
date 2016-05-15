#include "Tiger.h"
#include "Runtime/Engine/Classes/GameFramework/CheatManager.h"
#include "T_PlayerController.h"
#include "T_PlayerCharacter.h"

AT_PlayerCharacter::AT_PlayerCharacter()
{
	// Player defaults
	bIsSlomo = false;
	bPlayAttackAnim = false;
	TeamID = ET_Team::TEAM_Player;

	// Create music component
	Music = CreateDefaultSubobject<UAudioComponent>(TEXT("Music"));
	Music->SetRelativeLocation(FVector(.0f));
	Music->bIsMusic = true;
	Music->bAutoActivate = false;

	// Camera 64cm off ground
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->AttachParent = RootComponent;
	Camera->SetRelativeLocation(FVector(.0f, .0f, 64.f));

	// Viewmodel
	GetMesh()->AttachParent = Camera;
	GetMesh()->SetRelativeLocation(FVector(10.f, 4.f, -160.f));
	GetMesh()->SetRelativeRotation(FRotator(4.21506f, -14.892242f, .0f));//-.639038f)); 
	GetMesh()->SetHiddenInGame(true);

	// Bind overlap delegate
	OnActorBeginOverlap.AddDynamic(this, &AT_PlayerCharacter::OnBeginOverlap);
}

void AT_PlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

}

void AT_PlayerCharacter::OnConstruction(const FTransform &Transform)
{
	Super::OnConstruction(Transform);

	// vv This should be in the AI class
	// Loop attached actors
	// If one is a weapon:
	//		Detach it from us
	//		Save reference in weapon pointer
	//		Move it to hand position
	//		It will be picked up correctly on BeginPlay()
}

void AT_PlayerCharacter::StartJumping()
{
	Jump();
}

void AT_PlayerCharacter::PickupWeapon(class AT_Weapon *OtherWeapon)
{
	Super::PickupWeapon(OtherWeapon);
	GetMesh()->SetHiddenInGame(false);
}

void AT_PlayerCharacter::DropWeapon()
{
	Super::DropWeapon();
	GetMesh()->SetHiddenInGame(true);
}

void AT_PlayerCharacter::Die()
{
	bIsSlomo = false;
	Super::Die();
}

void AT_PlayerCharacter::OnBeginOverlap(AActor *OtherActor)
{
	Super::OnBeginOverlap(OtherActor);
}

void AT_PlayerCharacter::GetViewPoint(FRotator &OutRotation, FVector &OutLocation)
{
	OutRotation = Camera->GetComponentRotation();
	OutLocation = Camera->GetComponentLocation();
}

FVector AT_PlayerCharacter::GetAimVector()
{
	return Camera->GetComponentRotation().Vector();
}

/* Accessors */
// Get
bool AT_PlayerCharacter::GetIsSlomo() { return bIsSlomo; }
UCameraComponent *AT_PlayerCharacter::GetCamera() { return Camera; }
UAudioComponent *AT_PlayerCharacter::GetMusic() { return Music; }

// Set
void AT_PlayerCharacter::SetIsSlomo(bool bNew) { bIsSlomo = bNew; }