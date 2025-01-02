#include "GameModes/FSM/CCStateResolve.h"

#include "GameModes/CCGameMode.h"
#include "GameModes/CCGameState.h"
#include "GameModes/FSM/CCStateDrawingCards.h"
#include "Grid/CCGridManager.h"
#include "Grid/CCTile.h"
#include "Macro/CCLogMacro.h"
#include "Player/CCPlayerState.h"
#include "TileActor/CCTileUnit.h"
#include "TileActor/CCUnitMovementComponent.h"


void UCCStateResolve::OnEnterState()
{
	Super::OnEnterState();
	AreAllPlayerQueuesSent = false;
	DEBUG_WARNING_CATEGORY(LogFSM, "Number of Action queues = %i",GameMode->PlayerActions.Num());
}

void UCCStateResolve::OnStateTick(float DeltaTime)
{
	Super::OnStateTick(DeltaTime);
	if (!AreAllPlayerQueuesSent && GameMode->PlayerActions.Num() == GameMode->GetNumOfPlayersNeeded())
	{
		AreAllPlayerQueuesSent = true;
		OnAllPlayerQueuesSent();
	}
}

void UCCStateResolve::OnExitState()
{
	Super::OnExitState();
	
	FTileTypeDelegate TileTypeDelegate;
	auto Lambda = [this](ACCTile* Tile)
	{
		for(auto Unit : Tile->GetPieces())
		{
			//TODO : Add Score to Team
			//GameState->AddScore(Unit->GetTeam());
		}	
	};
	GameState->GetGridManager()->ApplyLambdaToTileType(ETileType::ScoreTile,TileTypeDelegate);
}

void UCCStateResolve::OnAllPlayerQueuesSent()
{
	for(auto Action : GameMode->PlayerActions)
	{
		for(auto Act : Action.Value)
		{
		}
	}
	StartNewAction();
}

void UCCStateResolve::StartNewAction()
{
	NumberOfActionResolved = 0;
	
	bool AreAllActionsResolved = true;
	for(auto& Queue : GameMode->PlayerActions)
	{
		if(!Queue.Value.IsEmpty())
		{
			AreAllActionsResolved = false;
		}
	}
	
	if(AreAllActionsResolved)
	{
		GameMode->GetFSM()->ChangeStateWithClass(UCCStateDrawingCards::StaticClass());
		return;	
	}

	
	for(auto& Queue : GameMode->PlayerActions)
	{
		if(Queue.Value.IsEmpty())
		{
			OnActionResolved(Queue.Key, FPlayerActionData(), nullptr);
			continue;
		};
		FPlayerActionData Action = Queue.Value[0];
		Queue.Value.RemoveAt(0);
		ACCTile* TargetTile = GameState->GetGridManager()->GetTile(Action.TargetCoord);
			
		//Movement Action
		if(!Action.MovementData.IsEmpty())
		{
			ACCTileUnit* Unit = Cast<ACCTileUnit>(TargetTile->GetPiece(Action.UnitID));

			//Related Unit has been Destroyed in previous Action
			if(!IsValid(Unit))
			{
				OnActionResolved(Queue.Key, Action, Unit);
				return;
			}
			else
			{
				FTileActorMovementDelegate TileActorMovementDelegate;
				TileActorMovementDelegate.BindDynamic(this, &UCCStateResolve::OnActionResolved);
				Unit->MovementComponent->StartMovement(Action.TargetCoord, Action, TileActorMovementDelegate, Queue.Key);
				
				TargetTile->RemovePiece(Unit);
				FIntPoint EndMovementCoordinates = Unit->CurrentCoordinates;
				for(auto MvtData : Action.MovementData) EndMovementCoordinates += MvtData.Direction; 
				ACCTile* EndMovementTile = GameState->GetGridManager()->GetTile(EndMovementCoordinates);
				check(EndMovementTile);
				EndMovementTile->MLC_AddPiece(Unit);
				Unit->CurrentCoordinates = EndMovementCoordinates;
			}
		}
		//Spawn Action
		else
		{
			const FRotator UnitRotation;
			const FVector UnitPosition = TargetTile->GetActorLocation() + FVector::UpVector * 20;
			FActorSpawnParameters UnitSpawnParams;
			UnitSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			UnitSpawnParams.bNoFail = true;
			
			ACCTileUnit* Unit = GetWorld()->SpawnActor<ACCTileUnit>(GetWorld()->GetGameState<ACCGameState>()->PieceClass, UnitPosition, UnitRotation, UnitSpawnParams);
			Unit->InitUnit(FInitilizationProperties(Action.TargetCoord, Queue.Key->GetTeam(), Action.UnitID, Action.CardData));
			
			//Delete Preview
			Queue.Key->GetPawn<ACCPlayerPawn>()->RPC_RemoveFirstActionClientElements();
 
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, Queue, Action, Unit]{OnActionResolved(Queue.Key, Action, Unit);}, 2, false, -1);
		}
	}
}

void UCCStateResolve::OnActionResolved(ACCPlayerState* PlayerState, FPlayerActionData LastAction, ACCPieceBase* LastPiece)
{
	if(IsValid(LastPiece))
	{
		ApplyActionEffects(PlayerState, LastAction, LastPiece, !LastAction.MovementData.IsEmpty());
	}
	NumberOfActionResolved++;
	
	if(NumberOfActionResolved == GameMode->GetNumOfPlayersNeeded())
	{
		FTimerHandle NewActionTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(NewActionTimerHandle, this, &UCCStateResolve::StartNewAction, 1, false, -1);
	}
}

void UCCStateResolve::ApplyActionEffects(ACCPlayerState* PlayerState, const FPlayerActionData& LastAction, ACCPieceBase* LastPiece, bool IsMovementAction)
{
	if(IsMovementAction)
	{
		ACCPlayerPawn* PlayerPawn = PlayerState->GetPawn<ACCPlayerPawn>();
		PlayerPawn->RPC_RemoveFirstActionClientElements();

		if(ACCTile* PieceTile = GameState->GetGridManager()->GetTile(LastPiece->CurrentCoordinates))
		{
			if(PieceTile->GetTileType() == ETileType::BonusTile)
			{
				GameMode->BonusTileMap[PieceTile] = LastPiece->GetTeam();
			}
		}
	}

	ACCTile* NewTile = GameState->GetGridManager()->GetTile(LastPiece->CurrentCoordinates);
	if(IsValid(NewTile->GetPiece(LastPiece->UnitGuid)))
	{
		TArray<ACCPieceBase*> DestroyedPieces;
		for(auto Unit : NewTile->GetPieces())
		{
			if(Unit->UnitGuid != LastAction.UnitID)
			{
				DestroyedPieces.Add(Unit);
			}
		}

		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, [DestroyedPieces]()
		{
			for(auto Piece : DestroyedPieces)
			{
				Piece->MLC_DestroyPiece();
			}
		},.5f, false, -1);
	}
}


