#pragma once

#include "GameFramework/Character.h"
#include "T_Weapon.h"
#include "T_CharacterBase.generated.h"

// Character teams
UENUM(BlueprintType)
enum class ET_Team : uint8
{
	TEAM_Neutral	UMETA(DisplayName = "Neutral"),
	TEAM_Player		UMETA(DisplayName = "Player"),
	TEAM_Enemy		UMETA(DisplayName = "Enemy")
};

// Base class for all characters (player or non)
UCLASS(Abstract, Blueprintable)
class TIGER_API AT_CharacterBase : public ACharacter
{
	GENERATED_BODY()
protected:
	// Generic character data
	int32 Health;
	int32 MaxHealth;
	float WalkSpeed;
	float RunSpeed;
	bool bIsAlive;
	bool bIsRunning;
	bool bIsAttacking;
	bool bHasWeapon;

	FName WeaponSocketName;		// Name of grip socket on pawn mesh
	class AT_Weapon *Weapon;	// Reference to carried weapon (should use a TWeakObjectPtr?)

public:
	// This character's team
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Character")
	TEnumAsByte<ET_Team> TeamID;

public:
	// Constructor
	AT_CharacterBase();

	// Base beginplay -- subclasses should call Super::BeginPlay() first
	virtual void BeginPlay() override;

	// Character interface
	UFUNCTION()
	virtual void Attack();
	UFUNCTION()
	virtual void PickupWeapon(AT_Weapon *OtherWeapon);
	UFUNCTION()
	virtual void DropWeapon();
	UFUNCTION()
	virtual void Damage(uint32 Amount);
	UFUNCTION()
	virtual void Heal(uint32 Amount);
	UFUNCTION()
	virtual void Die();
	UFUNCTION()
	virtual void OnBeginOverlap(AActor *OtherActor);
	UFUNCTION()
	virtual void GetViewPoint(FRotator &OutRotation, FVector &OutLocation);		// Similar function in AIController
	virtual FVector GetAimVector();

	// Getters
	UFUNCTION(BlueprintPure, Category = "Character")
	int32 GetHealth();
	UFUNCTION(BlueprintPure, Category = "Character")
	float GetWalkSpeed();
	UFUNCTION(BlueprintPure, Category = "Character")
	float GetRunSpeed();
	UFUNCTION(BlueprintPure, Category = "Character")
	bool GetIsAlive();
	UFUNCTION(BlueprintPure, Category = "Character")
	bool GetIsRunning();
	UFUNCTION(BlueprintPure, Category = "Character")
	bool GetIsAttacking();
	UFUNCTION(BlueprintPure, Category = "Character")
	bool GetHasWeapon();
	UFUNCTION(BlueprintPure, Category = "Character")
	AT_Weapon *GetWeapon();

	// Setters
	void SetIsAttacking(bool bNew);
	void SetIsRunning(bool bNew);
};
