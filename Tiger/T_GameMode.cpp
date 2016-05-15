#include "Tiger.h"
#include "T_GameMode.h"
#include "T_PlayerCharacter.h"
#include "T_PlayerController.h"

// Asset references
struct FConstructorStatics
{
	ConstructorHelpers::FObjectFinderOptional<UBlueprint> PlayerCharacterBP;
	ConstructorHelpers::FObjectFinderOptional<UBlueprint> PlayerControllerBP;

	FConstructorStatics()
	:	PlayerCharacterBP(TEXT("Blueprint'/Game/Blueprints/Player/PlayerCharacterBP.PlayerCharacterBP'")),
		PlayerControllerBP(TEXT("Blueprint'/Game/Blueprints/Player/PlayerControllerBP.PlayerControllerBP'"))
	{
	}
};

AT_GameMode::AT_GameMode()
{
	// Static blueprint references
	static FConstructorStatics CS;

	// Set player character class
	if (CS.PlayerCharacterBP.Get())
		DefaultPawnClass = Cast<UClass>(CS.PlayerCharacterBP.Get()->GeneratedClass);
	else
		DefaultPawnClass = AT_PlayerCharacter::StaticClass();

	// Set player controller class
	if (CS.PlayerControllerBP.Get())
		PlayerControllerClass = Cast<UClass>(CS.PlayerControllerBP.Get()->GeneratedClass);
	else
		PlayerControllerClass = AT_PlayerController::StaticClass();
}