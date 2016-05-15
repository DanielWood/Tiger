#pragma once

#include "T_Weapon.h"
#include "T_ProjectileLauncher.generated.h"

UCLASS()
class TIGER_API AT_ProjectileLauncher : public AT_Weapon
{
	GENERATED_BODY()
	

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float ProjectileForce;

public:
	// Constructor
	AT_ProjectileLauncher();

	// AT_Weapon interface
	virtual void Attack(FVector Direction, FVector Source) override;
	virtual bool IsWeaponReady() override;
};
