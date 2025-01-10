#include "GameModes/FSM/CCStateDrawingCards.h"
#include "GameModes/CCGameMode.h"
#include "GameModes/CCGameState.h"
#include "GameModes/FSM/CCFSM.h"
#include "GameModes/FSM/CCStatePlanification.h"
#include "Grid/CCGridManager.h"
#include "Grid/CCTile.h"
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

	if(GameState->IsGameOver) return;
	
	for(auto Player : Players)
	{
		ETeam PlayerTeam = Player->GetPlayerState<ACCPlayerState>()->GetTeam();
		int BonusPoints = 0;
		for(auto& TilePairing : GameMode->BonusTileMap)
		{
			if(TilePairing.Value == PlayerTeam)
			{
				BonusPoints++;
			}
		}
		
		Player->EndDrawDelegate.BindDynamic(this, &UCCStateDrawingCards::OnPlayerEndDraw);
		Player->RPC_DrawCards(GameMode->BaseNumberOfCardsToDraw + BonusPoints);
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
