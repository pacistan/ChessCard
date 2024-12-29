#include "GameModes/FSM/CCStatePlanification.h"

#include "GameModes/CCGameMode.h"
#include "GameModes/CCGameState.h"
#include "GameModes/FSM/CCFSM.h"
#include "GameModes/FSM/CCStateResolve.h"
#include "Player/CCPlayerPawn.h"
#include "Player/CCPlayerState.h"

void UCCStatePlanification::Initialization()
{
	Super::Initialization();
	CurrentTime = CCGameMode->GetTimeOfPlanniningPhase();
	
}

void UCCStatePlanification::OnEnterState()
{
	CCGameState->SetCurrentTimeOfPlanniningPhase(CurrentTime);
}

void UCCStatePlanification::OnStateTick(float DeltaTime)
{
	Super::OnStateTick(DeltaTime);
	CurrentTime -= DeltaTime;
	CCGameState->SetCurrentTimeOfPlanniningPhase(CurrentTime);
	
	if (CurrentTime <= 0) {
		for (auto Pawn : CCGameMode->GetPlayerPawns()) {
			Pawn->ForceEndTurn();
		}
		CCGameMode->GetFSM()->ChangeStateWithClass(UCCStateResolve::StaticClass());
	} else {
		TArray<APlayerState*> Players = CCGameState->PlayerArray;
		for(auto Player : Players) {
			if (ACCPlayerState* PlayerState = Cast<ACCPlayerState>(Player)) {
				if (!PlayerState->GetHasEndedTurn()) {
					return;
				}
			}
		}
	}

	CCGameMode->GetFSM()->ChangeStateWithClass(UCCStateResolve::StaticClass());
}

void UCCStatePlanification::OnExitState()
{
	// Ask all players to send their queue of actions to the server
	TArray<ACCPlayerPawn*> Players = CCGameMode->GetPlayerPawns();
	for(auto Player : Players) {
		Player->RPC_SendQueueOfAction();
	}
}
