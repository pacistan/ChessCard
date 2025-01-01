#include "GameModes/FSM/CCStateDrawingCards.h"

#include "GameModes/CCGameMode.h"
#include "GameModes/FSM/CCFSM.h"
#include "GameModes/FSM/CCStatePlanification.h"
#include "Kismet/GameplayStatics.h"
#include "Macro/CCLogMacro.h"
#include "Player/CCPlayerPawn.h"

void UCCStateDrawingCards::Initialization()
{
	Super::Initialization();
	Players = GameMode->GetPlayerPawns();

}

void UCCStateDrawingCards::OnEnterState()
{
	Super::OnEnterState();
	for(auto Player : Players) {
		Player->EndDrawDelegate.BindDynamic(this, &UCCStateDrawingCards::OnPlayerEndDraw);
		Player->RPC_DrawCards(GameMode->BaseNumberOfCardsToDraw);
		Player->RPC_ClearActions();
	}
	GameMode->PlayerActions.Empty();
}

void UCCStateDrawingCards::OnPlayerEndDraw(ACCPlayerPawn* Player)
{
	Player->EndDrawDelegate.Unbind();
	
	if(Players.Contains(Player)) {
		Players.Remove(Player);
	}
	
	if (Players.Num() == 0) {
		GameMode->GetFSM()->ChangeStateWithClass(UCCStatePlanification::StaticClass());
	}
}

void UCCStateDrawingCards::OnExitState()
{
	Super::OnExitState();
}
