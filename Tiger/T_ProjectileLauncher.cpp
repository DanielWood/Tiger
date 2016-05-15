#include "Tiger.h"
#include "T_ProjectileLauncher.h"

AT_ProjectileLauncher::AT_ProjectileLauncher()
{
	ProjectileForce = 1000.f;
}

void AT_ProjectileLauncher::Attack(FVector Direction, FVector Source) 
{
	if (!ProjectileClass || !ProjectileClass->IsValidLowLevelFast())
		return;

	AActor *Projectile = GetWorld()->SpawnActor(ProjectileClass, 
												&Source);
}

bool AT_ProjectileLauncher::IsWeaponReady() 
{
	return true; //false;
}