#include "GameModes/FSM/CCFSM.h"
#include "GameModes/CCGameMode.h"
#include "GameModes/FSM/CCBaseState.h"

UCCFSM::UCCFSM()
{
}

void UCCFSM::OnBeginPlay()
{
	Owner = Cast<ACCGameMode>(GetWorld()->GetAuthGameMode());
	ChangeStateWithClass(Owner->GetStartState());
}

void UCCFSM::OnTick(float DeltaTime)
{
	if(CurrentState != nullptr)
	{
		CurrentState->OnStateTick(DeltaTime);
	}
}

bool UCCFSM::ChangeStateWithClass(TSubclassOf<UCCBaseState> NewStateClass)
{
	if(NewStateClass == nullptr)
	{
		return false;
	}
	else if(CurrentState != nullptr)
	{
		CurrentState->OnExitState();
	}
	
	CurrentState = UCCBaseState::MakeStateWithClass(NewStateClass, Owner);
	CurrentState->Initialization(Owner);
	CurrentState->OnEnterState();
	return true;
}

