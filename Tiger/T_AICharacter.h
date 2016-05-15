#pragma once

#include "T_CharacterBase.h"
#include "T_AICharacter.generated.h"

// AI State enum
UENUM(BlueprintType, Category = "AI")
namespace ET_AIState
{
	enum State
	{
		Wander		UMETA(DisplayName = "Wandering"),	// Path following state (includes idling at points)
		Attack		UMETA(DisplayName = "Attacking"),	// Attacking an enemy
		Search		UMETA(DisplayName = "Searching")	// Searching for enemy after losing sight
		//Dead		UMETA(DisplayName = "Dead")			// Dead (using bIsAlive in CharacterBase instead)
	};
}

/* AI Pathing Sytem -- Work in progress! */
// AI waypoint type (WIP)
USTRUCT(BlueprintType)
struct FT_Waypoint
{
	GENERATED_USTRUCT_BODY()

	// Location of waypoint
	UPROPERTY(EditAnywhere, Category = "AI", Meta = (MakeEditWidget = true, DisplayName = "Waypoint Location"))
	FVector Point;

	// Duration to idle at this location (0 = no idle)
	UPROPERTY(EditAnywhere, Category = "AI", Meta = (DisplayName = "Idle Duration"))
	float IdleDuration;

	// Animation montage to play if idling at this point (Will set up a proper system integrated with animation blueprints later)
	UPROPERTY(EditAnywhere, Category = "AI", Meta = (DisplayName = "Idle Anim Montage"))
	UAnimMontage *IdleAnimMontage;

	// Allow path diversion from this point
	UPROPERTY(EditAnywhere, Category = "AI", Meta = (DisplayName = "Allow Path Diversion?"))
	bool bAllowPathDiversion;

	// Internal check for AICharacter construction script
	bool bIsInitialized;

	FT_Waypoint() : Point(FVector(200.f, .0f, .0f)), IdleDuration(.0f), bAllowPathDiversion(false), bIsInitialized(false)
	{
	}
};

// AI path type (WIP)
USTRUCT(BlueprintType)
struct FT_AIPath
{
	GENERATED_USTRUCT_BODY()

	// Waypoints array
	UPROPERTY(EditAnywhere, Category = "AI", Meta = (MakeEditWidget = true, DisplayName = "Waypoints"))
	TArray<struct FT_Waypoint> Waypoints;

	// Times to loop this path before unconditionally moving to the next one (A negative value will loop infinitely)
	UPROPERTY(EditAnywhere, Category = "AI", Meta = (DisplayName = "Path Loops"))
	int32 nPathLoops;

	// Chance of diversion at path end [0-1] (ignored if path is no longer looping)
	UPROPERTY(EditAnywhere, Category = "AI", Meta = (DisplayName = "Path End Diversion Chance"))
	float PathEndDiversionChance;

	// Chance of path diversion per-point [0-1] (ignored if bAllowPathDiversion is unchecked for point)
	UPROPERTY(EditAnywhere, Category = "AI", Meta = (DisplayName = "Diversion Chance Per-Point"))
	float DiversionChancePerPoint;

	FT_AIPath() : nPathLoops(0), PathEndDiversionChance(.0f), DiversionChancePerPoint(.0f)
	{
	}
};

/* T_AICharacter -- Tiger base for AI characters. */
UCLASS()
class TIGER_API AT_AICharacter : public AT_CharacterBase
{
	GENERATED_BODY()
public:
	// Behavior tree asset reference
	UPROPERTY(EditDefaultsOnly, Category = "AI")
	class UBehaviorTree *BehaviorTree;

	// Flag to trigger hit animation -- Would like to do this a better way
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadWrite, Category = "AI")
	bool bPlayHitAnim;

	// Teams this AI is enemies with (Does this belong in the character or the controller?)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
	TArray<TEnumAsByte<ET_Team>> EnemyTeams;

	// Teams this AI is friends with (AI should protect their friends if they can)
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "AI")
	TArray<TEnumAsByte<ET_Team>> FriendTeams;

	// Array of AI paths
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", Meta = (MakeEditWidget = true, DisplayName = "AI Paths"))
	TArray<struct FT_AIPath> AIPaths;

	// Keep track of our position in the AIPaths array
	uint8 PathIndex;
	uint8 WaypointIndices[256];

	// Waypoints will move with player if unset
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI", Meta = (DisplayName = "Keep Waypoint World Position?"))
	bool bKeepWaypointWorldPosition;

	// Radius of sight
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float SightRadius;

	// Lose sight radius (offset of SightRadius)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float LoseSightRadiusDelta;

	// Angle of peripheral vision
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float PeripheralVisionAngle;

	// Tick rate of AIPerceptionComponent
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float PerceptionTickRate;

public:
	// Constructor
	AT_AICharacter();

	// Engine interface
	//virtual void BeginPlay() override;
	virtual void OnConstruction(const FTransform &Transform) override;

	// AT_CharacterBase interface
	virtual void Die() override;
	virtual void Attack() override;
	virtual void GetViewPoint(FRotator &OutRotation, FVector &OutLocation) override;
	virtual FVector GetAimVector() override;

	// Offset all waypoints
	inline void AddPathsLocalOffset(FVector Offset);

	// Hypothetical functions:
	// bool IsFriendlyTo(AT_CharacterBase *OtherCharacter); Generic
	// bool IsFriendlyTo(AT_AICharacter *OtherAI); AI specific
};
