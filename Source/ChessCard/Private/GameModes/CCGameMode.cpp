#include "GameModes/CCGameMode.h"
#include "GameModes/CCGameState.h"
#include "GameModes/FSM/CCFSM.h"
#include "Kismet/GameplayStatics.h"
#include "Macro/CCLogMacro.h"
#include "Player/CCPlayerController.h"
#include "Player/CCPlayerPawn.h"
#include "Player/CCPlayerState.h"

ACCGameMode::ACCGameMode(const FObjectInitializer& ObjectInitializer)
{
	GameStateClass = ACCGameState::StaticClass();
	PlayerControllerClass = ACCPlayerController::StaticClass();
	PlayerStateClass = ACCPlayerState::StaticClass();
	DefaultPawnClass = ::ACCPlayerPawn::StaticClass();
}

void ACCGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	OnUserPostLogin.Broadcast(NewPlayer);
}

void ACCGameMode::StartPlaySequence()
{
	FSM = NewObject<UCCFSM>(this);
	FSM->OnBeginPlay();
}

TArray<ACCPlayerPawn*> ACCGameMode::GetPlayerPawns()
{
	TArray<ACCPlayerPawn*> Players;
	for(auto PlayerState :  GetGameState<AGameStateBase>()->PlayerArray)
	{
		ACCPlayerPawn* Player = Cast<ACCPlayerPawn>(PlayerState->GetPawn());
		if(Player)
		{
			Players.Add(Player);
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Found %i Players"), Players.Num());
	return Players;
}
