#include "Tiger.h"
#include "Classes/Kismet/KismetMathLibrary.h"
#include "T_PlayerController.h"

AT_PlayerController::AT_PlayerController()
{
	// Player controller defaults
	TimeDilation = 1.f;
	SlomoTimeDilation = .33f;
	DefaultFOV = 70.f;
	SlomoFOV = 115.f;
	DefaultFringeIntensity = 0.f;
	SlomoFringeIntensity = 3.f;
	SlomoInterpSpeed = 10.f;

	TimeDilationTarget = TimeDilation;
	FOVTarget = DefaultFOV;

	AimSensitivity = FVector2D(1.f, 1.f);
	InputYawScale = AimSensitivity.X;
	InputPitchScale = AimSensitivity.Y;

	bContinueAttacking = false;
}

void AT_PlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	InputComponent->BindAxis("MoveForward", this, &AT_PlayerController::MoveForward);
	InputComponent->BindAxis("MoveRight", this, &AT_PlayerController::MoveRight);

	InputComponent->BindAxis("Turn", this, &AT_PlayerController::AddYawInput);
	InputComponent->BindAxis("LookUp", this, &AT_PlayerController::AddPitchInput);
	//InputComponent->BindAxis("LookUp", this, &AT_PlayerController::LookUp);

	InputComponent->BindAction("Run", IE_Pressed, this, &AT_PlayerController::StartRunning);
	InputComponent->BindAction("Run", IE_Released, this, &AT_PlayerController::StopRunning);

	InputComponent->BindAction("Jump", IE_Pressed, this, &AT_PlayerController::StartJumping);
	InputComponent->BindAction("Jump", IE_Released, this, &AT_PlayerController::StopJumping);

	InputComponent->BindAction("Attack", IE_Pressed, this, &AT_PlayerController::StartAttacking);
	InputComponent->BindAction("Attack", IE_Released, this, &AT_PlayerController::StopAttacking);

	InputComponent->BindAction("Slomo", IE_Pressed, this, &AT_PlayerController::StartSlomo);
	InputComponent->BindAction("Slomo", IE_Released, this, &AT_PlayerController::StopSlomo);

	InputComponent->BindAction("DropWeapon", IE_Pressed, this, &AT_PlayerController::DropWeapon);

	// Debug input
	InputComponent->BindAction("DEBUG_UpdateViewModelRotation", IE_Pressed, this, &AT_PlayerController::DEBUG_UpdateViewModelRotation);
	InputComponent->BindAction("Music", IE_Pressed, this, &AT_PlayerController::ToggleMusic);
}

void AT_PlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Interpolation on tick
	InterpTick();
}

void AT_PlayerController::Possess(APawn *InPawn)
{
	Super::Possess(InPawn);

	// Save character pointer
	PlayerCharacter = Cast<AT_PlayerCharacter>(InPawn);

	if (PlayerCharacter.IsValid())
	{
		Camera = PlayerCharacter->GetCamera();			// Save camera pointer
		Camera->FieldOfView = DefaultFOV;				// Set FOV
	}

	// Find PostProcessVolume pointer if not already set in a derived blueprint
	if (!PostProcess.IsValid())
	{
		// Get first actor
		TActorIterator<APostProcessVolume> FirstPP(GetWorld());
		
		if (FirstPP && FirstPP->IsValidLowLevel())
		{
			PostProcess = *FirstPP;
			DefaultFringeIntensity = PostProcess->Settings.SceneFringeIntensity;
		}
	}
}

void AT_PlayerController::UnPossess()
{
	if (PlayerCharacter.IsValid())
		PlayerCharacter->Die();		// Kill PlayerCharacter incase it is still alive

	Super::UnPossess();
}

void AT_PlayerController::MoveForward(float Value)
{
	if (PlayerCharacter.IsValid())
		PlayerCharacter->AddMovementInput(PlayerCharacter->GetActorForwardVector(), Value);
}

void AT_PlayerController::MoveRight(float Value)
{
	if (PlayerCharacter.IsValid())
		PlayerCharacter->AddMovementInput(PlayerCharacter->GetActorRightVector(), Value);
}

// REMOVEME (Unused input delegate)
void AT_PlayerController::LookUp(float Value)
{
	if (!PlayerCharacter.IsValid())
		return;

	// Apply vertical mouse movement to camera pitch
	FRotator Delta = FRotator(Value * AimSensitivity.Y, 0.f, 0.f);
	FRotator New = Camera->RelativeRotation + Delta;
	New.Pitch = FMath::ClampAngle(New.Pitch, -90.f, 90.f); 
	Camera->SetRelativeRotation(New);
}

void AT_PlayerController::StartRunning()
{
	if (PlayerCharacter.IsValid())
	{
		PlayerCharacter->SetIsRunning(true);
		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = PlayerCharacter->GetRunSpeed();
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("WalkSpeed = %f"), PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed));
	}
}

void AT_PlayerController::StopRunning()
{
	if (PlayerCharacter.IsValid())
	{
		PlayerCharacter->SetIsRunning(false);
		PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed = PlayerCharacter->GetWalkSpeed();
		GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("WalkSpeed = %f"), PlayerCharacter->GetCharacterMovement()->MaxWalkSpeed));
	}
}

void AT_PlayerController::StartJumping()
{
	if (PlayerCharacter.IsValid())
		PlayerCharacter->Jump();
}

void AT_PlayerController::StopJumping()
{
	if (PlayerCharacter.IsValid())
		PlayerCharacter->StopJumping();
}

void AT_PlayerController::StartSlomo()
{
	if (PlayerCharacter.IsValid())
	{
		FOVTarget = SlomoFOV;
		TimeDilationTarget = SlomoTimeDilation;
		FringeIntensityTarget = SlomoFringeIntensity;
		PlayerCharacter->SetIsSlomo(true);
	}
}

void AT_PlayerController::StopSlomo()
{
	if (PlayerCharacter.IsValid())
	{
		FOVTarget = DefaultFOV;
		TimeDilationTarget = 1.f;
		FringeIntensityTarget = DefaultFringeIntensity;
		PlayerCharacter->SetIsSlomo(false);
	}
}

void AT_PlayerController::StartAttacking()
{
	if (PlayerCharacter.IsValid())
	{
		// Dont attack if we are mid-attacking
		if (PlayerCharacter->GetIsAttacking())
			return;

		// Only attack if we have a weapon
		if (PlayerCharacter->GetHasWeapon() && PlayerCharacter->GetWeapon())
		{
			AT_Weapon *Weapon = PlayerCharacter->GetWeapon();

			// Dont attack if weapon is unready (out of ammo)
			if (!Weapon->IsWeaponReady())
				return;

			if (Weapon->bIsAutomatic)
				bContinueAttacking = true;		// Start chain of attacks
			else
				GetWorldTimerManager().SetTimer(AttackTimer, this, &AT_PlayerController::EnableAttackInput, Weapon->FireRate, false);

			Attack();
		}
	}
}

void AT_PlayerController::StopAttacking()
{
	// Stop automatic timer after next attack (ideally it should stop before next attack [should be easy to do in above function])
	bContinueAttacking = false;

	// Re-enable attack input for automatic weapons after attack delay is finished
	if (GetWorldTimerManager().IsTimerActive(AttackTimer))
	{
		float Remaining = GetWorldTimerManager().GetTimerRemaining(AttackTimer);
		GetWorldTimerManager().ClearTimer(AttackTimer);
		
		if (Remaining > KINDA_SMALL_NUMBER)
			GetWorldTimerManager().SetTimer(AttackTimer, this, &AT_PlayerController::EnableAttackInput, Remaining, false);
		else
			EnableAttackInput();
	}
}

void AT_PlayerController::EnableAttackInput()
{
	PlayerCharacter->SetIsAttacking(false);
}

void AT_PlayerController::Attack()
{
	if (!PlayerCharacter.IsValid() || !PlayerCharacter->GetHasWeapon())
		return;

	PlayerCharacter->SetIsAttacking(true);
	PlayerCharacter->Attack();	// <-- This function should start the attack animation (Even if it is still playing)
	
	if (bContinueAttacking && PlayerCharacter->GetWeapon())
		GetWorldTimerManager().SetTimer(AttackTimer, this, &AT_PlayerController::Attack, PlayerCharacter->GetWeapon()->FireRate, false);

}

// Drop weapon (doesnt always fire?)
void AT_PlayerController::DropWeapon()
{
	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Green, "Controller: DropWeapon() fired -- Did your gun drop?");
	if (PlayerCharacter.IsValid())
		PlayerCharacter->DropWeapon();
}

// Toggle in-game music (development function, actual setting will be Music Volume in the UI menu)
void AT_PlayerController::ToggleMusic()
{
	static bool bIsPlaying = true;

	PlayerCharacter->GetMusic()->SetVolumeMultiplier((float)bIsPlaying);
	bIsPlaying = !bIsPlaying;
	
}

// Dev function, remove in final release, or apply an interp?
void AT_PlayerController::DEBUG_UpdateViewModelRotation()
{
	if (!PlayerCharacter.IsValid())
		return;

	// Get character aim coordinates
	FVector EyeLoc;
	FRotator AimRot;
	PlayerCharacter->GetViewPoint(AimRot, EyeLoc);

	// Points A and B
	FVector Start = PlayerCharacter->GetWeapon()->GetActorLocation();
	FVector End = EyeLoc + AimRot.Vector() * PlayerCharacter->GetWeapon()->Range;

	// Rotation difference
	FRotator R1 = PlayerCharacter->GetWeapon()->GetActorRotation();
	FRotator R2 = UKismetMathLibrary::FindLookAtRotation(Start, End);
	FRotator DeltaR = R2 - R1;

	// Apply to mesh rotation
	FRotator NewMeshRot = PlayerCharacter->GetMesh()->GetComponentRotation() + DeltaR;
	NewMeshRot.Roll = 0.f;
	PlayerCharacter->GetMesh()->SetWorldRotation(NewMeshRot);
}

// TODO: Use boolean checks for interp conditions
// Have a function on a timer every ~.05 seconds that updates these booleans
// OR:
// Set a boolean marking each value as outdated whenever we change one
// If that flag is set: Perform the interp and periodically check the distance between the current and the target (if close enough unset the flag)
inline void AT_PlayerController::InterpTick()
{
	if (!PlayerCharacter.IsValid())
		return;

	float DeltaTime = GetWorld()->GetDeltaSeconds();

	// Interp slomo
	if (FMath::Abs(TimeDilation - TimeDilationTarget) > KINDA_SMALL_NUMBER)
	{
		TimeDilation = FMath::FInterpTo(TimeDilation, TimeDilationTarget, DeltaTime, SlomoInterpSpeed);
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), TimeDilation);
		GEngine->AddOnScreenDebugMessage(-1, .01f, FColor::Blue, FString::Printf(TEXT("TimeDilation: %f"), TimeDilation));
	}

	// Interp FOV
	static float FOVInterpSpeed = SlomoInterpSpeed * .5f;
	if (FMath::Abs(Camera->FieldOfView - FOVTarget) > 1.e-2f)	// Need not be as accurate as TimeDilation
	{
		Camera->FieldOfView = FMath::FInterpTo(Camera->FieldOfView, FOVTarget, DeltaTime, FOVInterpSpeed);
		GEngine->AddOnScreenDebugMessage(-1, .01f, FColor::Green, FString::Printf(TEXT("FOV: %f"), Camera->FieldOfView));
	}

	// Interp fringe intensity
	if (PostProcess.IsValid())	// Use a boolean to avoid tick spam
	{
		static float FringeInterpSpeed = SlomoInterpSpeed * .33f;
		static float *FringeIntensity = &PostProcess->Settings.SceneFringeIntensity;
		if (FMath::Abs(*FringeIntensity - FringeIntensityTarget) > 1.e-3f)
		{
			*FringeIntensity = FMath::FInterpTo(*FringeIntensity, FringeIntensityTarget, DeltaTime, FringeInterpSpeed);
			GEngine->AddOnScreenDebugMessage(-1, .01f, FColor::Yellow, FString::Printf(TEXT("FringeIntensity: %f"), *FringeIntensity));
		}
	}
}
