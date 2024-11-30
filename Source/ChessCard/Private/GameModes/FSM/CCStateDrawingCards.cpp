#include "GameModes/FSM/CCStateDrawingCards.h"

#include "GameModes/CCGameMode.h"
#include "GameModes/FSM/CCFSM.h"
#include "GameModes/FSM/CCStateMain.h"
#include "Kismet/GameplayStatics.h"
#include "Player/CCPlayerPawn.h"

void UCCStateDrawingCards::Initialization(ACCGameMode* InGameMode)
{
	Super::Initialization(InGameMode);
	Players = InGameMode->GetPlayerPawns();
}

void UCCStateDrawingCards::OnEnterState()
{
	Super::OnEnterState();
	for(auto Player : Players)
	{
		Player->EndDrawDelegate.BindDynamic(this, &UCCStateDrawingCards::OnPlayerEndDraw);
		Player->DrawCards(5);
	}

}

void UCCStateDrawingCards::OnPlayerEndDraw(ACCPlayerPawn* Player)
{
	if(Players.Contains(Player))
	{
		Players.Remove(Player);
	}
	if(Players.Num() == 0)
	{
		GameMode->GetFSM()->ChangeStateWithClass(UCCStateMain::StaticClass());
	}
}
