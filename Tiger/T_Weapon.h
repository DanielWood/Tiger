#pragma once

#include "GameFramework/Actor.h"
#include "T_Weapon.generated.h"

// Weapon type (shape) enum
UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	WP_Rifle		UMETA(DisplayName = "Rifle"),
	WP_Pistol		UMETA(DisplayName = "Pistol"),
	WP_Melee		UMETA(DisplayNAme = "Melee")
};

// Base weapon class
UCLASS(Abstract)
class TIGER_API AT_Weapon : public AActor
{
	GENERATED_BODY()

public:
	// Gameplay parameters
	bool bIsPickedUp;
	bool bIgnorePickups;
	float ThrowVelocity;

	FTimerHandle PickupTimer;

	// Plan to replace this with a proper system that chooses a hit particle based on the surface
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	UParticleSystem *HitParticles;

	// How long to reject pickups after being dropped (~1s seems reasonable)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	float DropIgnoreTime;

	// Damage per hit
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	int32 Damage;

	// How far it shoots
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float Range;

	// Time between attacks
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	float FireRate;

	// Is the weapon automatic
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	bool bIsAutomatic;

	// Weapon name
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FString WeaponName;

	// Weapon category
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Weapon")
	TEnumAsByte<EWeaponType> WeaponType;

	// Mesh component
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Weapon")
	class USkeletalMeshComponent *Mesh;

	// Reference to character carrying this weapon
	class APawn *Carrier;
	
	// Constructor
	AT_Weapon();

	// Weapon interface
	UFUNCTION()
	virtual void Attack(FVector Direction, FVector Source);
	UFUNCTION()
	virtual bool IsWeaponReady();

	// Sets bIgnorePickups to false
	UFUNCTION()
	void BecomePickup();

	// Weapon functions
	//void Pickup(ATigerCharacter *Owner);
	void Drop(FVector ThrowAt, bool bThrow = true);
};
