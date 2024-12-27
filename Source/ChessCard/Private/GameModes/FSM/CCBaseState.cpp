#include "GameModes/FSM/CCBaseState.h"
#include "Macro/CCLogMacro.h"
#include "GameModes/CCGameMode.h"
#include "GameModes/CCGameState.h"

UCCBaseState* UCCBaseState::MakeStateWithClass(TSubclassOf<UCCBaseState> NewStateClass, UObject* Outer)
{
	return NewObject<UCCBaseState>(Outer, NewStateClass);
}

void UCCBaseState::Initialization()
{
	if (ACCGameMode* GameMode = GetWorld()->GetAuthGameMode<ACCGameMode>()) {
		CCGameMode = GameMode;
	}

	if (ACCGameState* GameState = GetWorld()->GetGameState<ACCGameState>()) {
		CCGameState = GameState;
	}
}

void UCCBaseState::OnEnterState()
{
	DEBUG_LOG_CATEGORY(LogFSM,"Enter State: %s", *GetClass()->GetName())
}

void UCCBaseState::OnStateTick(float DeltaTime)
{
	DEBUG_LOG_CATEGORY(LogFSM,"Tick State: %s", *GetClass()->GetName())
}

void UCCBaseState::OnExitState()
{
	DEBUG_LOG_CATEGORY(LogFSM,"Exit State: %s", *GetClass()->GetName())
}