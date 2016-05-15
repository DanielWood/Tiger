#pragma once

#include "AIController.h"
#include "BehaviorTree/BehaviorTreeComponent.h"
#include "Perception/AIPerceptionComponent.h"
#include "Perception/AISenseConfig_Sight.h"
#include "T_AICharacter.h"
#include "T_AIController.generated.h"

/* T_AIController -- Tiger AI controller. */
UCLASS()
class TIGER_API AT_AIController : public AAIController
{
	GENERATED_BODY()
public:
	// Behavior tree component
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "AI")
	class UBehaviorTreeComponent *BehaviorTree;

protected:
	// Reference to possessed T_AICharacter
	TWeakObjectPtr<class AT_AICharacter> AICharacter;

	// Sight perception config
	UAISenseConfig_Sight *SightConfig;

	// Blackboard keys
	uint8 TargetEnemyKeyID;
	uint8 TargetPointKeyID;
	uint8 IdleDurationKeyID;
	uint8 AIStateKeyID;

	// The AI's current state
	UPROPERTY(VisibleAnywhere, Category = "AI")
	TEnumAsByte<ET_AIState::State> State;

	TSet<AT_CharacterBase*> PerceivedEnemies;
	TSet<AT_CharacterBase*> PerceivedFriends;

public:
	// Constructor
	AT_AIController();

	// AController Interface
	virtual void Possess(APawn *InPawn) override;
	virtual void UnPossess() override;

	// Updates target point in blackboard (To be called inside a BTTask/Service)
	UFUNCTION(BlueprintCallable, Category = "AI")
	void UpdateTargetPoint();

	// Perception routine
	UFUNCTION()
	void OnPerceptionUpdated(TArray<AActor*> PerceivedActors);

	// Getters
	UFUNCTION(BlueprintPure, Category = "AI")
	ET_AIState::State GetState();
};