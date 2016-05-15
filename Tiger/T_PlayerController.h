#pragma once

#include "GameFramework/PlayerController.h"
#include "T_PlayerCharacter.h"
#include "T_PlayerController.generated.h"

/* T_PlayerController -- Tiger player controller */
UCLASS()
class TIGER_API AT_PlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	// Timer to limit attacks to current weapon's fire rate
	FTimerHandle AttackTimer;
	bool bContinueAttacking;

protected:
	// References
	TWeakObjectPtr<class AT_PlayerCharacter> PlayerCharacter;
	TWeakObjectPtr<class UCameraComponent> Camera;

	// Postprocessing volume reference
	TWeakObjectPtr<class APostProcessVolume> PostProcess;

	// Mouse input sensitivity
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay)
	FVector2D AimSensitivity;

	// World time dilation
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = Gameplay)
	float TimeDilation;

	// World time dilation when in slomo
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay)
	float SlomoTimeDilation;

	// Default FOV
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay)
	float DefaultFOV;

	// FOV when in slomo
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay)
	float SlomoFOV;

	// Default postprocess fringe intensity
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay)
	float DefaultFringeIntensity;

	// Postprocess fringe intensity when slomo
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay)
	float SlomoFringeIntensity;

	// Speed of interpolation between regular speed and slomo
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = Gameplay)
	float SlomoInterpSpeed;

	// Interpolation targets
	float TimeDilationTarget;
	float FOVTarget;
	float FringeIntensityTarget;

	// Input axis delegates
	UFUNCTION()
	void MoveForward(float Value);
	UFUNCTION()
	void MoveRight(float Value);
	UFUNCTION()
	void LookUp(float Value);

	// Input action delegates
	UFUNCTION()
	void StartRunning();
	UFUNCTION()
	void StopRunning();
	UFUNCTION()
	void StartJumping();
	UFUNCTION()
	void StopJumping();
	UFUNCTION()
	void StartSlomo();
	UFUNCTION()
	void StopSlomo();
	UFUNCTION()
	void StartAttacking();
	UFUNCTION()
	void StopAttacking();
	UFUNCTION()
	void DropWeapon();	// Wrapper around AT_PlayerCharacter::DropWeapon()
	UFUNCTION()
	void ToggleMusic();
	UFUNCTION()
	void DEBUG_UpdateViewModelRotation();	// Faces viewmodel the right way -- REMOVEME when viewmodels no longer need debugging

	// Timer delegates
	UFUNCTION()
	void Attack();		// Wrapper around AT_PlayerCharacter::Attack()
	UFUNCTION()
	void EnableAttackInput();

	// Slomo transition tick
	inline void InterpTick();

public:
	// Constructor
	AT_PlayerController();

	// APlayerController interface
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupInputComponent() override;
	virtual void Possess(APawn *InPawn) override;
	virtual void UnPossess() override;
};
