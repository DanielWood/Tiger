#include "Tiger.h"
#include "T_AIController.h"
#include "T_AICharacter.h"

AT_AICharacter::AT_AICharacter()
{
	AIControllerClass = AT_AIController::StaticClass();

	// NPC defaults
	bPlayHitAnim = false;
	bKeepWaypointWorldPosition = true;
	SightRadius = 2000.f;
	LoseSightRadiusDelta = 500.f;
	PeripheralVisionAngle = 90.f;
	PerceptionTickRate = .4f;

	// Initialize array indices to 0
	PathIndex = 0;
	for (int i = 0; i < 256; i++) WaypointIndices[i] = 0;

	// Set mesh location and rotation
	GetMesh()->SetRelativeLocation(FVector(0.f, 0.f, -85.f));
	GetMesh()->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
}

// Doesnt work?
void AT_AICharacter::OnConstruction(const FTransform &Transform)
{
	Super::OnConstruction(Transform);

	// TODO: New waypoints should appear next to the previous ones
	// Maybe with a smart trace to avoid going through walls

	// TODO: Make waypoints hold their world position when character is rotated

	// Previous/Current location
	static FVector LastPosition = Transform.GetLocation();
	FVector Position = Transform.GetLocation();

	// Hold waypoint world position
	// Used to work but now doesnt?
	if (bKeepWaypointWorldPosition && Position != LastPosition)
	{
		AddPathsLocalOffset(LastPosition - Position);
		LastPosition = Position;
	}
}

inline void AT_AICharacter::AddPathsLocalOffset(FVector Offset)
{
	for (int32 i = 0; i < AIPaths.Num(); i++)
		for (int32 j = 0; j < AIPaths[i].Waypoints.Num(); j++)
			AIPaths[i].Waypoints[j].Point += Offset;
}

void AT_AICharacter::Die()
{
	// Stop behavior tree on controller
	AT_AIController *AIConRef = Cast<AT_AIController>(GetController());
	if (AIConRef && AIConRef->IsValidLowLevel())
		if (AIConRef->BehaviorTree->IsRunning())
			AIConRef->BehaviorTree->StopTree(EBTStopMode::Safe);

	Super::Die();
}

void AT_AICharacter::Attack()
{
	Super::Attack();
}

void AT_AICharacter::GetViewPoint(FRotator &OutRotation, FVector &OutLocation)
{
	// TODO (or not, check out the similar functions in AController)
	Super::GetViewPoint(OutRotation, OutLocation);
}


FVector AT_AICharacter::GetAimVector()
{
	return Super::GetAimVector();
}