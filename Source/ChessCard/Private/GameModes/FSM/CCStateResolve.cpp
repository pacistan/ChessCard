#include "GameModes/FSM/CCStateResolve.h"

#include "GameModes/CCGameMode.h"
#include "Macro/CCLogMacro.h"

void UCCStateResolve::OnAllPlayerQueuesSent()
{
	DEBUG_WARNING("Start Of Resolve Phase");
}

void UCCStateResolve::OnEnterState()
{
	Super::OnEnterState();
	AreAllPlayerQueuesSent = false;
}

void UCCStateResolve::OnStateTick(float DeltaTime)
{
	Super::OnStateTick(DeltaTime);
	if(AreAllPlayerQueuesSent == false && GameMode->GetPlayerActionsQueue().Num() == GameMode->GetNumOfPlayersNeeded())
	{
		AreAllPlayerQueuesSent = true;
		OnAllPlayerQueuesSent();
	}
}

