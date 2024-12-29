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
		CCGameMode->GetFSM()->ChangeStateWithClass(UCCStateResolve::StaticClass());
		return;
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

	
	DEBUG_LOG_SCREEN_SIMPLE("All Player have ended their turn, need to change state to resolve");
	CCGameMode->GetFSM()->ChangeStateWithClass(UCCStateResolve::StaticClass());
}

void UCCStatePlanification::OnExitState()
{
	// Force all player to end their turn
	for (auto Pawn : CCGameMode->GetPlayerPawns()) {
		Pawn->ForceEndTurn();
	}
	
	// Ask all players to send their queue of actions to the server
	TArray<ACCPlayerPawn*> Players = CCGameMode->GetPlayerPawns();
	for(auto Player : Players) {
		Player->RPC_SendQueueOfAction();
	}
}
