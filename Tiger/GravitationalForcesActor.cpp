#include "Tiger.h"
#include "GravitationalForcesActor.h"


// Sets default values
AGravitationalForcesActor::AGravitationalForcesActor()
{
	GravityScale = 1000.f;

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AGravitationalForcesActor::BeginPlay()
{
	Super::BeginPlay();
	
	// Add all actors of each target class to the TargetActors array
	for (auto &Class : TargetClasses)
	{
		for (TActorIterator<AActor> Iter(GetWorld(), Class); Iter; ++Iter)
		{
			struct FGravityTarget NewTarget = { 0 };
			NewTarget.Actor = *Iter;
			TargetActors.Add(NewTarget);
		}
	}

	// Apply initial force
	for (auto &Target : TargetActors)
	{
		if (!&Target || !Target.Actor || !Target.Actor->IsValidLowLevelFast())
			continue;

		UPrimitiveComponent *ActorRoot = Cast<UPrimitiveComponent>(Target.Actor->GetRootComponent());
		if (!ActorRoot || !ActorRoot->IsValidLowLevelFast())
			continue;

		ActorRoot->AddImpulse(Target.InitialForce, NAME_None, true);
		ActorRoot->AddAngularImpulse(Target.InitialTorque, NAME_None, true);
	}
}

// Called every frame
void AGravitationalForcesActor::Tick( float DeltaTime )
{
	// Apply gravity
	for (auto &Target : TargetActors)
	{
		if (!Target.Actor || !Target.Actor->IsValidLowLevelFast())
			break;

		UPrimitiveComponent *ActorRoot = Cast<UPrimitiveComponent>(Target.Actor->GetRootComponent());
		if (!ActorRoot)
			continue;

		for (auto &OtherTarget : TargetActors)
		{
			if(!OtherTarget.Actor || !OtherTarget.Actor->IsValidLowLevelFast() || OtherTarget.Actor == Target.Actor)
				continue;

			UPrimitiveComponent *OtherActorRoot = Cast<UPrimitiveComponent>(OtherTarget.Actor->GetRootComponent());
			if (!OtherActorRoot)
				continue;

			FVector DeltaLocation = Target.Actor->GetActorLocation() - OtherTarget.Actor->GetActorLocation();

			// Gravity is directly proportional to mass
			float ForceScale = ActorRoot->GetMass() * OtherActorRoot->GetMass();

			// and inversely proportional to distance^2
			float Distance = DeltaLocation.Size();
			ForceScale /= Distance * Distance; 

			// Create force vector
			DeltaLocation.Normalize();
			FVector Force = ForceScale * DeltaLocation * GravityScale;

			// Apply constant force on tick
			OtherActorRoot->AddForce(Force * DeltaTime);
		}
	
	}

	Super::Tick( DeltaTime );
}

