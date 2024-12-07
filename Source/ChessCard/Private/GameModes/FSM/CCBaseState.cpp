#include "GameModes/FSM/CCBaseState.h"

UCCBaseState* UCCBaseState::MakeStateWithClass(TSubclassOf<UCCBaseState> NewStateClass, UObject* Outer)
{
	return NewObject<UCCBaseState>(Outer, NewStateClass);
}

void UCCBaseState::Initialization(ACCGameMode* InGameMode)
{
	GameMode = InGameMode;
}

void UCCBaseState::OnEnterState()
{
	UE_LOG(LogTemp, Warning, TEXT("Enter State: %s"), *GetClass()->GetName())
}

void UCCBaseState::OnStateTick(float DeltaTime)
{
}

void UCCBaseState::OnExitState()
{
}