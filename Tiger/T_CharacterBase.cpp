#include "Tiger.h"
#include "Perception/AIPerceptionSystem.h"
#include "Perception/AISense_Sight.h"
#include "T_CharacterBase.h"


/* Generic character base methods */
AT_CharacterBase::AT_CharacterBase()
{
	// Character defaults
	MaxHealth = 100;
	Health = MaxHealth;
	TeamID = ET_Team::TEAM_Neutral;
	WalkSpeed = 640.f;
	RunSpeed = 1024.f;
	bIsAlive = true;
	bIsRunning = false;
	bIsAttacking = false;
	bHasWeapon = false;
	WeaponSocketName = FName("hand_r_gun");

	// Receive yaw input from controller
	bUseControllerRotationYaw = true;
	bUseControllerRotationPitch = false;
	bUseControllerRotationRoll = false;	

	// Configure character movement
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->JumpZVelocity = 560;
	GetCharacterMovement()->bUseFlatBaseForFloorChecks = true;
	GetCharacterMovement()->bOrientRotationToMovement = false;
	GetCharacterMovement()->bUseControllerDesiredRotation = false;
	GetCharacterMovement()->SetWalkableFloorAngle(50.f);

	// Can tick
	PrimaryActorTick.bCanEverTick = true;
}

void AT_CharacterBase::BeginPlay()
{
	Super::BeginPlay();

	// Register characters as sight stimuli
	UAIPerceptionSystem::RegisterPerceptionStimuliSource(GetWorld(), UAISense_Sight::StaticClass(), this);

	// Pick up any weapon that was set in the editor
	// Can this even be done?
	//if (Weapon.IsValid())
//	{
//		PickupWeapon(Weapon.Get());
//	}
}

void AT_CharacterBase::Attack()
{
	if (!bHasWeapon || !Weapon)
		return;

	FRotator Direction;
	FVector EyeLocation;
	GetActorEyesViewPoint(EyeLocation, Direction);

	Weapon->Attack(Direction.Vector(), EyeLocation);
}

void AT_CharacterBase::PickupWeapon(AT_Weapon *OtherWeapon)
{
	// Drop current weapon (if there is one)
	DropWeapon();

	GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Yellow, "Character: Just picked up a weapon");
	// Pick up the new weapon
	Weapon = OtherWeapon;
	Weapon->Mesh->SetSimulatePhysics(false);
	Weapon->AttachRootComponentTo(GetMesh(), WeaponSocketName, EAttachLocation::SnapToTarget, true);
	Weapon->bIsPickedUp = true;
	Weapon->Mesh->bGenerateOverlapEvents = false;
	bHasWeapon = true;
}

void AT_CharacterBase::DropWeapon()
{
	bHasWeapon = false;

	if (Weapon && Weapon->IsValidLowLevel())
	{
		FVector ThrowAt = GetAimVector();
		Weapon->Drop(ThrowAt, true);
		Weapon->Mesh->bGenerateOverlapEvents = true;
	}

	Weapon = NULL;
}

// Deal damage to the character
void AT_CharacterBase::Damage(uint32 Amount)
{
	if (bIsAlive)
	{
		Health = FMath::Clamp<int32>(Health - Amount, 0, MaxHealth);

		if (Health == 0)
			Die();
	}
}

// Return health to character
void AT_CharacterBase::Heal(uint32 Amount)
{
	Health = FMath::Clamp<int32>(Health + Amount, 0, MaxHealth);
}

// Put character in dead state
void AT_CharacterBase::Die()
{
	DropWeapon();
	Health = 0;
	bIsAttacking = bIsRunning = bIsAlive = false;
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, TEXT("I'm Dead"));
}

void AT_CharacterBase::OnBeginOverlap(AActor *OtherActor)
{
	// Pick up weapons
	AT_Weapon *OtherWeapon = Cast<AT_Weapon>(OtherActor);
	if (OtherWeapon && OtherWeapon->IsValidLowLevel())
		if(!OtherWeapon->bIgnorePickups)
			PickupWeapon(OtherWeapon);
		else
			GEngine->AddOnScreenDebugMessage(-1, 2.f, FColor::Red, "Weapon is ignoring pickups");
}


/* Dummy definitions */
void AT_CharacterBase::GetViewPoint(FRotator &OutRotation, FVector &OutLocation) { }
FVector AT_CharacterBase::GetAimVector() { return GetController()->GetActorForwardVector(); }

/* Accessors */
// Get
int32 AT_CharacterBase::GetHealth() { return Health; }
float AT_CharacterBase::GetWalkSpeed() { return WalkSpeed; }
float AT_CharacterBase::GetRunSpeed() { return RunSpeed; }
bool AT_CharacterBase::GetIsAlive() { return bIsAlive; }
bool AT_CharacterBase::GetIsRunning() { return bIsRunning; }
bool AT_CharacterBase::GetIsAttacking() { return bIsAttacking; }
bool AT_CharacterBase::GetHasWeapon() { return bHasWeapon; }
AT_Weapon *AT_CharacterBase::GetWeapon() { return Weapon; }

// Set
void AT_CharacterBase::SetIsRunning(bool bNew) { bIsRunning = bNew; }
void AT_CharacterBase::SetIsAttacking(bool bNew) { bIsAttacking = bNew; }
