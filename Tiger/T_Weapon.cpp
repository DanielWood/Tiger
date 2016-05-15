#include "Tiger.h"
#include "T_Weapon.h"

// Sets default values
AT_Weapon::AT_Weapon()
{
	// Weapon defaults
	bIsPickedUp = false;
	bIsAutomatic = false;
	bIgnorePickups = false;
	DropIgnoreTime = 1.f;
	ThrowVelocity = 500.f;

	// Set up mesh component
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionProfileName("OverlapOnlyPawn");
	Mesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	Mesh->SetSimulatePhysics(true);
	Mesh->bGenerateOverlapEvents = true;
	RootComponent = Mesh;
}

// Drop weapon
void AT_Weapon::Drop(FVector ThrowAt, bool bThrow)
{
	DetachRootComponentFromParent();
	Mesh->SetSimulatePhysics(true);

	if (bThrow)
	{
		FVector ThrowImpulse = ThrowAt * ThrowVelocity;
		Mesh->AddImpulse(ThrowImpulse, NAME_None, true);
	}

	bIsPickedUp = false;
	bIgnorePickups = true;
	GetWorldTimerManager().SetTimer(PickupTimer, this, &AT_Weapon::BecomePickup, DropIgnoreTime, false);
}

// Stop ignoring pickups
void AT_Weapon::BecomePickup()
{
	bIgnorePickups = false;
}

// Dummy definitions
void AT_Weapon::Attack(FVector Direction, FVector Source) {}
bool AT_Weapon::IsWeaponReady() { return true; }