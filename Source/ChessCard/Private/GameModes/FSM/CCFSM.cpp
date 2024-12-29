#include "GameModes/FSM/CCFSM.h"
#include "GameModes/CCGameMode.h"
#include "GameModes/CCGameState.h"
#include "GameModes/FSM/CCBaseState.h"
#include "GameModes/FSM/CCStateDrawingCards.h"
#include "GameModes/FSM/CCStatePlanification.h"
#include "GameModes/FSM/CCStateResolve.h"
#include "Macro/CCLogMacro.h"

UCCFSM::UCCFSM()
{
	StateMap.Add(UCCStateDrawingCards::StaticClass(), EGameState::DrawingCards);
	StateMap.Add(UCCStatePlanification::StaticClass(), EGameState::Plannification);
	StateMap.Add(UCCStateResolve::StaticClass(), EGameState::Resolve);
	// StateMap.Add(UCCStateEndGame::StaticClass(), EGameState::EndGame);
}

void UCCFSM::OnTick(float DeltaTime)
{
	if(CurrentState != nullptr) {
		CurrentState->OnStateTick(DeltaTime);
	}
}

bool UCCFSM::ChangeStateWithClass(TSubclassOf<UCCBaseState> NewStateClass)
{
	if(NewStateClass == nullptr) {
		return false;
	} else if (IsValid(CurrentState)) {
		CurrentState->OnExitState();
	}
	
	if (ACCGameState* GameState = GetWorld()->GetGameState<ACCGameState>()) {
		GameState->SetCurrentState(StateMap[NewStateClass]);
	}
	
	CurrentState = UCCBaseState::MakeStateWithClass(NewStateClass, GetOuter());
	CurrentState->Initialization();
	CurrentState->OnEnterState();
	return true;
}

