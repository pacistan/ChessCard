#include "GameModes/FSM/CCStatePlanification.h"

#include "Components/SplineComponent.h"
#include "GameModes/CCGameMode.h"
#include "GameModes/CCGameState.h"
#include "GameModes/FSM/CCFSM.h"
#include "GameModes/FSM/CCStateResolve.h"
#include "Player/CCPlayerPawn.h"
#include "Player/CCPlayerState.h"




void UCCStatePlanification::Initialization()
{
	Super::Initialization();
	CurrentTime = GameMode->GetTimeOfPlanniningPhase();
	ReadyPlayers.Reserve(GameMode->GetNumOfPlayersNeeded());
}

void UCCStatePlanification::OnEnterState()
{
	GameState->SetCurrentTimeOfPlanniningPhase(CurrentTime);
	for(auto Player : GameState->PlayerArray) {
		Cast<ACCPlayerState>(Player)->EndTurnDelegate.BindDynamic(this, &UCCStatePlanification::PlayerEndTurnCallback);
	}
}

void UCCStatePlanification::OnStateTick(float DeltaTime)
{
	Super::OnStateTick(DeltaTime);
	CurrentTime -= DeltaTime;
	GameState->SetCurrentTimeOfPlanniningPhase(CurrentTime);
	
	if (CurrentTime <= 0)
	{
		// Force all player to end their turn
		for (auto Pawn : GameMode->GetPlayerPawns()) {
			Pawn->ForceEndTurn();
		}
		GameMode->GetFSM()->ChangeStateWithClass(UCCStateResolve::StaticClass());
		return;
	}
}

void UCCStatePlanification::OnExitState()
{
	GameState->SetCurrentTimeOfPlanniningPhase(GameMode->GetTimeOfPlanniningPhase());
	// Ask all players to send their queue of actions to the server
	TArray<ACCPlayerPawn*> Players = GameMode->GetPlayerPawns();
	for(auto Player : Players) {
		Player->RPC_SendQueueOfAction();
	}
}

void UCCStatePlanification::PlayerEndTurnCallback(ACCPlayerState* Player, bool IsEndTurn)
{
	if(IsEndTurn)
	{
		if(!ReadyPlayers.Contains(Player))
		{
			ReadyPlayers.Add(Player);
			if(ReadyPlayers.Num() == GameMode->GetNumOfPlayersNeeded())
			{
				GameMode->GetFSM()->ChangeStateWithClass(UCCStateResolve::StaticClass());
			}
		}
		else
		{
			DEBUG_ERROR("Trying to register already registered player to Player End Turn List");
		}
	}
	else
	{
		if(ReadyPlayers.Contains(Player))
		{
			ReadyPlayers.Remove(Player);
		}
		else
		{
			DEBUG_ERROR("Trying to unregister player to Player End Turn List while not beeing registered");
		}
	}
}


