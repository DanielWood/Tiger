#include "Tiger.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "BehaviorTree/BehaviorTree.h"
#include "BehaviorTree/Blackboard/BlackboardKeyAllTypes.h"
#include "T_AIController.h"

AT_AIController::AT_AIController()
{
	State = ET_AIState::Wander;		// AI is in wandering state by default

	// Setup behavior components
	BehaviorTree = CreateDefaultSubobject<UBehaviorTreeComponent>(TEXT("BehaviourTree"));
	Blackboard = CreateDefaultSubobject<UBlackboardComponent>(TEXT("Blackboard"));

	// Setup sight config
	SetPerceptionComponent(*CreateDefaultSubobject<UAIPerceptionComponent>(TEXT("Perception")));
	SightConfig = CreateDefaultSubobject<UAISenseConfig_Sight>(TEXT("SightConfig"));

	// Detect everyone for the time-being until there is better documentation/updates to this feature
	SightConfig->DetectionByAffiliation.bDetectFriendlies = true;
	SightConfig->DetectionByAffiliation.bDetectEnemies = true;
	SightConfig->DetectionByAffiliation.bDetectNeutrals = true;

	// Setup perception component
	GetPerceptionComponent()->ConfigureSense(*SightConfig);
	GetPerceptionComponent()->SetDominantSense(SightConfig->StaticClass());
	GetPerceptionComponent()->OnPerceptionUpdated.AddDynamic(this, &AT_AIController::OnPerceptionUpdated);
}

void AT_AIController::Possess(APawn *InPawn)
{
	Super::Possess(InPawn);

	// Check that pawn is child of AT_AICharacter
	AT_AICharacter *InCharacter = Cast<AT_AICharacter>(InPawn);
	if (InCharacter && InCharacter->IsValidLowLevel())
	{
		// Save pointer
		AICharacter = InCharacter;

		// Make sure AI is friends with its own team
		if (!AICharacter->FriendTeams.Contains(AICharacter->TeamID))
			AICharacter->FriendTeams.Add(AICharacter->TeamID);

		// Load perception config
		GetPerceptionComponent()->PrimaryComponentTick.TickInterval = AICharacter->PerceptionTickRate; // Correct?
		SightConfig->SightRadius = AICharacter->SightRadius;
		SightConfig->LoseSightRadius = AICharacter->SightRadius + AICharacter->LoseSightRadiusDelta;
		SightConfig->PeripheralVisionAngleDegrees = AICharacter->PeripheralVisionAngle;

		// Initialize behavior
		if (AICharacter->BehaviorTree)
		{
			// Initialize Blackboard
			Blackboard->InitializeBlackboard(*AICharacter->BehaviorTree->BlackboardAsset);
			TargetEnemyKeyID = Blackboard->GetKeyID("Target Enemy");
			TargetPointKeyID = Blackboard->GetKeyID("Target Point");
			IdleDurationKeyID = Blackboard->GetKeyID("Idle Duration");
			AIStateKeyID = Blackboard->GetKeyID("AI State");

			// Convert paths to world space
			FVector Forwards = AICharacter->GetActorForwardVector();
			FVector Right = AICharacter->GetActorRightVector();
			for (int32 i = 0; i < AICharacter->AIPaths.Num(); i++)
				for (int32 j = 0; j < AICharacter->AIPaths[i].Waypoints.Num(); j++)
					{
						FVector *Point = &AICharacter->AIPaths[i].Waypoints[j].Point;
						FVector X = Point->X * Forwards;
						FVector Y = Point->Y * Right;
						X.Z = Y.Z = Point->Z;
						*Point = X + Y;
					}
			AICharacter->AddPathsLocalOffset(AICharacter->GetActorLocation());

			// Start running the tree
			BehaviorTree->StartTree(*AICharacter->BehaviorTree);

			// Get first target point
			UpdateTargetPoint();
		}
	}
}

void AT_AIController::UnPossess()
{
	if (AICharacter.IsValid())
		AICharacter->Die();		// Kill character incase it is still alive

	Super::UnPossess();
}

void AT_AIController::UpdateTargetPoint()
{
	GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Blue, FString("UpdateTargetPoint()"));
	if (!AICharacter.IsValid())
		return;

	// If there is no set paths for this AI
	if (!AICharacter->AIPaths.Num() || !AICharacter->AIPaths[0].Waypoints.Num())
	{
		// Use current location as target point
		Blackboard->SetValue<UBlackboardKeyType_Vector>(TargetPointKeyID, AICharacter->GetActorLocation());
		return;
	}

	// Avoid spaghetti
	uint8 *PathIndex = &AICharacter->PathIndex;
	uint8 *WaypointIndex = &AICharacter->WaypointIndices[*PathIndex];

	// Increment and wrap-around array indices (Is there a FMath function suitable for this??)
	if (++*WaypointIndex >= AICharacter->AIPaths[*PathIndex].Waypoints.Num())
	{
		*WaypointIndex = 0;

		if (++*PathIndex >= AICharacter->AIPaths.Num())
			*PathIndex = 0;
	}

	// Get next point
	FVector Next = AICharacter->AIPaths[*PathIndex].Waypoints[*WaypointIndex].Point;
	float IdleDuration = AICharacter->AIPaths[*PathIndex].Waypoints[*WaypointIndex].IdleDuration;

	// Save point in blackboard
	Blackboard->SetValue<UBlackboardKeyType_Vector>(TargetPointKeyID, Next);
	Blackboard->SetValue<UBlackboardKeyType_Float>(IdleDurationKeyID, IdleDuration);

}

// Perception routine
void AT_AIController::OnPerceptionUpdated(TArray<AActor*> PerceivedActors)
{
	for (int32 i = 0; i < PerceivedActors.Num(); i++)
	{
		FActorPerceptionBlueprintInfo ActorInfo;
		GetPerceptionComponent()->GetActorsPerception(PerceivedActors[i], ActorInfo);
		TWeakObjectPtr<AT_CharacterBase> PerceivedCharacter = Cast<AT_CharacterBase>(ActorInfo.Target);

		// Only think about Tiger Characters
		if (!PerceivedCharacter.IsValid())
			return;

		// If the character is visible
		if (ActorInfo.LastSensedStimuli[0].IsActive())
		{
			if (AICharacter->EnemyTeams.Contains(PerceivedCharacter->TeamID))	// If character is an enemy 
			{
				if (!PerceivedEnemies.Contains(PerceivedCharacter.Get()))		// That we just saw
				{
					// Add new enemy to set
					PerceivedEnemies.Add(PerceivedCharacter.Get());

					// Find nearest enemy
					float ShortestDist = MAX_FLT;
					TWeakObjectPtr<AT_CharacterBase> NearestEnemy = PerceivedCharacter;
					for (auto Iter(PerceivedEnemies.CreateIterator()); Iter; ++Iter)
					{
						float Distance = FVector::Dist(AICharacter->GetActorLocation(), (*Iter)->GetActorLocation());
						if (Distance < ShortestDist)
						{
							NearestEnemy = *Iter;
							ShortestDist = Distance;
						}
					}

					// Set nearest enemy in blackboard
					Blackboard->SetValue<UBlackboardKeyType_Object>(TargetEnemyKeyID, NearestEnemy.Get());

					// Set AI state to attack
					Blackboard->SetValue<UBlackboardKeyType_Enum>(AIStateKeyID, ET_AIState::Attack);
				}
			}
			else	// If the Enemy is no longer visible
			if (PerceivedEnemies.Contains(PerceivedCharacter.Get()))
			{
				// Remove it from the set
				PerceivedEnemies.Remove(PerceivedCharacter.Get());

				// Search for enemies if none are visible
				if (PerceivedEnemies.Num() < 1)
					Blackboard->SetValue<UBlackboardKeyType_Enum>(AIStateKeyID, ET_AIState::Search);
			}
		}
	}
}

// Getters
ET_AIState::State AT_AIController::GetState() { return State; }