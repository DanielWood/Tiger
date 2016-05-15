#pragma once

#include "GameFramework/Actor.h"
#include "GravitationalForcesActor.generated.h"

USTRUCT(BlueprintType)
struct FGravityTarget
{
	GENERATED_USTRUCT_BODY()
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	AActor *Actor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	FVector InitialForce;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	FVector InitialTorque;
};

/* Experimental gravity manager. Allows a secondary gravity force to be applied to select actors. */
UCLASS()
class TIGER_API AGravitationalForcesActor : public AActor
{
	GENERATED_BODY()
public:
	// Akin to Newton's constant
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	float GravityScale;

	// Classes to always apply gravity on
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	TArray<TSubclassOf<AActor>> TargetClasses;

	// Target actors to apply gravity on
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gravity")
	TArray<struct FGravityTarget> TargetActors;

public:	
	AGravitationalForcesActor();

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
};
