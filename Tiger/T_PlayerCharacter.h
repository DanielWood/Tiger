#pragma once

#include "T_CharacterBase.h"
#include "T_PlayerCharacter.generated.h"

/* T_PlayerCharacter -- Tiger player character. */
UCLASS()
class TIGER_API AT_PlayerCharacter : public AT_CharacterBase
{
	GENERATED_BODY()
private:
	// Gameplay camera component
	UPROPERTY(VisibleAnywhere, Category = Character)
	class UCameraComponent *Camera;

	// Audio component for later
	UPROPERTY(VisibleAnywhere, Category = Character)
	class UAudioComponent *Music;

	// Is (the world) in slomo?
	UPROPERTY(VisibleAnywhere, Category = Character)
	bool bIsSlomo;
public:
	// Flag to trigger attack animation
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Character)
	bool bPlayAttackAnim;

public:
	// Constructor
	AT_PlayerCharacter();

	// ACharacter::Jump wrapper with wall run code -- TODO
	void StartJumping();

	// AActor interface
	virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform &Transform) override;

	// AT_CharacterBase interface
	virtual void PickupWeapon(class AT_Weapon *OtherWeapon) override;
	virtual void DropWeapon() override;
	virtual void Die() override;
	virtual void OnBeginOverlap(class AActor *OtherActor) override;
	virtual void GetViewPoint(FRotator &OutRotation, FVector &OutLocation) override;
	virtual FVector GetAimVector() override;

	// Getters (Should be UFUNCTIONS?)
	bool GetIsSlomo();
	UCameraComponent *GetCamera();
	UAudioComponent *GetMusic();

	// Setters
	void SetIsSlomo(bool bNew);
};
