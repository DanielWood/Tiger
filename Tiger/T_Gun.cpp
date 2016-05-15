#include "Tiger.h"
#include "T_CharacterBase.h"
#include "T_Gun.h"


// Sets default values
AT_Gun::AT_Gun()
{
	PrimaryActorTick.bCanEverTick = true;

	// Default Gameplay Parameters
	MagSize = 32;
	Rounds = MagSize;
	Range = 15748.f;		// Default range
	Damage = 100;
	FireRate = .25f;
}

// Construction script
void AT_Gun::OnConstruction(const FTransform &Transform)
{
	// Keep rounds full when changing MagSize in editor
	static float LastMagSize = MagSize;
	if (MagSize != LastMagSize)
	{
		Rounds = MagSize;
		LastMagSize = MagSize;
	}
}

// Fire round
void AT_Gun::Attack(FVector Direction, FVector Source)
{
	if (!IsWeaponReady())
		return;

	// Calculate trace points
	FVector Origin = Mesh->GetSocketLocation("Muzzle") + Direction * 5;
	FVector Target = Source + Direction * Range;

	// Configure query parameters
	FHitResult OutHit(ForceInit);
	FCollisionQueryParams Params;
	FCollisionObjectQueryParams ObjParams;
	Params.AddIgnoredActor(this);
	Params.bFindInitialOverlaps = true;
	ObjParams.AddObjectTypesToQuery(ECC_Pawn);
	ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);
	ObjParams.AddObjectTypesToQuery(ECC_WorldDynamic);
	ObjParams.AddObjectTypesToQuery(ECC_EngineTraceChannel1);

	// Remove round from mag
	Rounds--;

	// Do the trace
	GetWorld()->LineTraceSingleByObjectType(OutHit, Origin, Target, ObjParams, Params);
	DrawDebugLine(GetWorld(), Origin, (OutHit.IsValidBlockingHit() ? OutHit.ImpactPoint : Target), FColor::Green, true, 1, 0, .5f);

	// Skip damage part if we didnt hit anything
	if (!OutHit.IsValidBlockingHit())
		return;

	// Hit particles


	// Deal damage if we hit a character
	AT_CharacterBase *HitCharacter = Cast<AT_CharacterBase>(OutHit.GetActor());

	if (HitCharacter && HitCharacter->IsValidLowLevelFast())
		HitCharacter->Damage(Damage);
}

bool AT_Gun::IsWeaponReady()
{
	return (Rounds > 0 && Rounds <= MagSize);
}