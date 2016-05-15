#pragma once

#include "T_Weapon.h"
#include "T_Gun.generated.h"

UCLASS()
class TIGER_API AT_Gun : public AT_Weapon
{
	GENERATED_BODY()

public:
	// Magazine size
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 MagSize;

	// Remaining rounds (<=Mag Size)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 Rounds;

	// TODO: Tweakable bullet spread
	
	// Constructor
	AT_Gun();

	// Construction script
	virtual void OnConstruction(const FTransform &Transform) override;

	// AT_Weapon interface
	virtual void Attack(FVector Direction, FVector Source) override;
	virtual bool IsWeaponReady() override;
};
