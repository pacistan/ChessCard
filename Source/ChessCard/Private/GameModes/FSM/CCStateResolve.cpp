#include "GameModes/FSM/CCStateResolve.h"

#include "GameModes/CCEffectManagerComponent.h"
#include "GameModes/CCGameMode.h"
#include "GameModes/CCGameState.h"
#include "GameModes/FSM/CCStateDrawingCards.h"
#include "Grid/CCGridManager.h"
#include "Grid/CCTile.h"
#include "Kismet/GameplayStatics.h"
#include "Macro/CCLogMacro.h"
#include "Player/CCPlayerState.h"
#include "TileActor/CCTileUnit.h"
#include "TileActor/CCUnitMovementComponent.h"


void UCCStateResolve::OnEnterState()
{
	Super::OnEnterState();
	AreAllPlayerQueuesSent = false;

	TArray<AActor*> Units;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACCPieceBase::StaticClass(), Units);
	for(const auto& Unit : Units)
	{
		auto Piece = Cast<ACCTileUnit>(Unit);
		Piece->SetIsStunned(false);
	}
	
	//UnStun Units
	/*FTileTypeDelegate TileTypeDelegate;
	auto Lambda = [](ACCTile* Tile)
	{
		auto Pieces = Tile->GetPieces();
		for(int i = 0; i < Pieces.Num() - 1; i++)
		{
			if(ACCTileUnit* Unit = Cast<ACCTileUnit>(Pieces[i]))
			{
				DEBUG_LOG("Unit Unstunned Name : %s", *Unit->GetName());
				Unit->SetIsStunned(false);
			}
			if(i == 0)
			{
				Pieces[0]->RPC_SetVisible();
				Pieces[0]->SetActorHiddenInGame(false);
			}
		}	
	};
	TileTypeDelegate.BindLambda(Lambda);
	GameState->GetGridManager()->ApplyLambdaToTileType(ETileType::Unit, TileTypeDelegate);*/
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

	//Increment Divine Anger Counter
	{
		TArray<AActor*> Units;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACCPieceBase::StaticClass(), Units);
		for(const auto& Unit : Units)
		{
			auto Piece = Cast<ACCTileUnit>(Unit);
			Piece->IsMoved = false;
		}
		
		FTileTypeDelegate TileTypeDelegate;
		auto Lambda = [this](ACCTile* Tile)
		{
			for(const auto& Piece : Tile->GetPieces())
			{
				if(ACCTileUnit* Unit = Cast<ACCTileUnit>(Piece))
				{
					Unit->DivineAngerCounter++;
					Unit->MLC_OnDivineCounterChange();
					Unit->IsMoved = false;
					//if(Tile->GetTileType() == ETileType::ScoreTile)
					//{
					//	GameState->AddPointToPlayer(GameState->GetPlayerStateOfTeam(Unit->Team));
					//}
				}
			}	
		};
		TileTypeDelegate.BindLambda(Lambda);
		GameState->GetGridManager()->ApplyLambdaToTileType(ETileType::Unit, TileTypeDelegate);

		FTileTypeDelegate ScoreTileDelegate;
		auto LambdaScore = [this](ACCTile* Tile)
		{
			for(const auto& Piece : Tile->GetPieces())
			{
				if(ACCTileUnit* Unit = Cast<ACCTileUnit>(Piece))
				{
 					if(Tile->GetTileType() == ETileType::ScoreTile)
					{
						GameState->AddPointToPlayer(GameState->GetPlayerStateOfTeam(Unit->Team));
					}
				}
			}	
		};
		ScoreTileDelegate.BindLambda(LambdaScore);
		GameState->GetGridManager()->ApplyLambdaToTileType(ETileType::ScoreTile, ScoreTileDelegate);

		FPlayerScore PScore = GameState->GetScores();
		TArray<ACCPlayerState*> WinningPStates;
		for(int i = 0; i < PScore.Score.Num(); i++)
		{
			if(PScore.Score[i] >= GameMode->NumberOfPointsNeededForVictory)
			{
				WinningPStates.Add(Cast<ACCPlayerState>(PScore.PlayerStates[i]));
			}
		}

		if(!WinningPStates.IsEmpty())
		{
			GameState->IsGameOver = true;
			auto WinningState = WinningPStates[FMath::RandRange(0, WinningPStates.Num() - 1)];
			for(auto Pawn : GameMode->GetPlayerPawns())
			{
				Pawn->RPC_ActivateVictoryScreen(WinningState->GetTeam());
			}
		}
		
	}
}

void UCCStateResolve::OnAllPlayerQueuesSent()
{
	StartNewAction();
}

void UCCStateResolve::StartNewAction()
{
	NumberOfActionResolved = 0;
	
	for(auto TilePair : KillerTiles)
	{
		TilePair.Key->IsAboutToReceivePiece = false;
	}
	KillerTiles.Empty();
	SlaughterTiles.Empty();
	LastActions.Empty();
	
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

		if(Action.TargetCoord == FIntPoint(-1, -1))
		{
			OnActionResolved(Queue.Key, Action, nullptr);
			continue;
		}
		
		//Movement Action
		if(!Action.MovementData.IsEmpty())
		{
			ACCTileUnit* Unit = Cast<ACCTileUnit>(TargetTile->GetPiece(Action.UnitID));

			//Related Unit has been Destroyed in previous Action
			if(!IsValid(Unit) || Unit->GetIsStunned())
			{
				OnActionResolved(Queue.Key, Action, Unit);
				continue;
			}
			else
			{
				FTileActorMovementDelegate TileActorMovementDelegate;
				TileActorMovementDelegate.BindDynamic(this, &UCCStateResolve::OnActionResolved);
				Unit->MovementComponent->StartMovement(Action.TargetCoord, Action, TileActorMovementDelegate, Queue.Key);

				TargetTile->MLC_RemovePiece(Unit);
				FIntPoint EndMovementCoordinates = Unit->CurrentCoordinates;
				for(auto MvtData : Action.MovementData)
				{
					EndMovementCoordinates += MvtData.Direction; 
					if(MvtData.MovementType == EMovementType::Stoppable)
					{
						break;	
					}
				}
				ACCTile* EndMovementTile = GameState->GetGridManager()->GetTile(EndMovementCoordinates);
				check(EndMovementTile);
				Unit->PreviousCoordinates = Unit->CurrentCoordinates;
				Unit->CurrentCoordinates = EndMovementCoordinates;
				//EndMovementTile->MLC_AddPiece(Unit);
				EndMovementTile->IsAboutToReceivePiece = true;
				if(KillerTiles.Contains(EndMovementTile))
				{
					KillerTiles[EndMovementTile].Add(Unit);
					SlaughterTiles.FindOrAdd(EndMovementTile);
				}
				else
				{
					KillerTiles.Add(EndMovementTile, TArray<ACCPieceBase*>{Unit});
				}
				LastActions.Add(Unit, Action);
			}
		}
		//TriggerEmbrasement
		else if(Action.CardData.GetRow<FCardData>("")->EffectType == EEffectType::Embrasement)
		{
			ACCTileUnit* Unit = Cast<ACCTileUnit>(TargetTile->GetPiece(Action.UnitID));
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, Queue, Action, Unit]{OnActionResolved(Queue.Key, Action, Unit);}, 2, false, -1);
			LastActions.Add(Unit, Action);
		}
		//Spawn Action
		else if(IsValid(TargetTile))
		{
			const FRotator UnitRotation;
			const FVector UnitPosition = TargetTile->GetActorLocation() + FVector::UpVector * 20;
			FActorSpawnParameters UnitSpawnParams;
			UnitSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			UnitSpawnParams.bNoFail = true;

			ACCPieceBase* KilledUnit = nullptr;
			if(!TargetTile->GetPieces().IsEmpty())
			{
				KilledUnit = TargetTile->GetPieces()[0];
			}
			
			ACCTileUnit* Unit = GetWorld()->SpawnActor<ACCTileUnit>(GetWorld()->GetGameState<ACCGameState>()->PieceClass, UnitPosition, UnitRotation, UnitSpawnParams);
			Unit->InitUnit(FInitilizationProperties(Action.TargetCoord, Queue.Key->GetTeam(), Action.UnitID, Action.CardData, false));
			
			//Delete Preview
			Queue.Key->GetPawn<ACCPlayerPawn>()->RPC_RemoveFirstActionClientElements();


			if(IsValid(KilledUnit))
			{
				GameState->GetEffectManager()->TriggerResolveEffect(
			false, Unit->CardDataRowHandle, Unit, TArray<ACCTile*>(), EEffectTriggerType::OnKill, TArray<ACCPieceBase*>{KilledUnit}, FIntPoint(), Action);
				GameState->GetEffectManager()->TriggerResolveEffect(
			false, KilledUnit->CardDataRowHandle, KilledUnit, TArray<ACCTile*>(), EEffectTriggerType::OnDeath, TArray<ACCPieceBase*>{Unit}, FIntPoint(), FPlayerActionData());
				TargetTile->GetPieces()[0]->MLC_DestroyPiece();	
			}
			
			FTimerHandle TimerHandle;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this, Queue, Action, Unit]{OnActionResolved(Queue.Key, Action, Unit);}, GameMode->UnitSpawnActionDuration, false, -1);
		}
	}
}

void UCCStateResolve::OnActionResolved(ACCPlayerState* PlayerState, FPlayerActionData LastAction, ACCPieceBase* LastPiece)
{
	if(IsValid(LastPiece))
	{
		bool IsMovementAction = !LastAction.MovementData.IsEmpty() || LastAction.CardData.GetRow<FCardData>("")->EffectType == EEffectType::Embrasement;
		ApplyActionEffects(PlayerState, LastAction, LastPiece, IsMovementAction);
	}
	NumberOfActionResolved++;
	
	if(NumberOfActionResolved == GameMode->GetNumOfPlayersNeeded())
	{
		BloodSlaughterAndDeath();
		
		FTimerHandle NewActionTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(NewActionTimerHandle, this, &UCCStateResolve::StartNewAction, GameMode->TimerBetweenActions, false, -1);
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
			if(PieceTile->GetTileType() == ETileType::BonusTile  && !SlaughterTiles.Contains(PieceTile))
			{
				GameMode->BonusTileMap[PieceTile] = LastPiece->GetTeam();
				PieceTile->MLC_UpdateBonusTileColor(LastPiece->GetTeam());
			}
		}

		//Movement Proc
		ACCTileUnit* Unit = Cast<ACCTileUnit>(LastPiece);
		FIntPoint Coordinate = Unit->PreviousCoordinates;
		TArray<ACCTile*> EffectTiles;
		FIntPoint Direction = FIntPoint();
		for(auto& Mvt : LastAction.MovementData)
		{
			Coordinate += Mvt.Direction;
			if(Mvt.MovementType == EMovementType::ApplyEffect)
			{
				ACCTile* Tile = GameState->GetGridManager()->GetTile(Coordinate);
				if(IsValid(Tile))
				{
					EffectTiles.Add(Tile);
				}
			}
			else if(Mvt.MovementType == EMovementType::Stoppable)
			{
				Direction = Mvt.Direction;
			}
		}
		GameState->GetEffectManager()->TriggerResolveEffect(LastAction.IsDivineAnger, LastPiece->CardDataRowHandle,
			LastPiece, EffectTiles, EEffectTriggerType::OnMove, TArray<ACCPieceBase*>(), Direction, LastAction);
	}
}

void UCCStateResolve::BloodSlaughterAndDeath()
{
	FKillDeathDelegate KillDelegate = FKillDeathDelegate();
	FKillDeathDelegate DeathDelegate = FKillDeathDelegate();
	TArray<ACCPieceBase*> DyingPieces;
	for (auto TilePair : KillerTiles)
	{
		for(auto Piece : TilePair.Value)
		{
			TilePair.Key->MLC_AddPiece(Piece);
		}
		if(TilePair.Key->GetPieces().Num() > 1)
		{
			for (auto Piece : TilePair.Key->GetPieces())
			{
				if (SlaughterTiles.Contains(TilePair.Key) && TilePair.Value.Contains(Piece))
				{
					AddKillLambda(KillDelegate, TilePair.Key, Piece);
					AddDeathLambda(DeathDelegate, TilePair.Key, Piece);
					DyingPieces.Add(Piece);
				}
				else if (TilePair.Value.Contains(Piece))
				{
					AddKillLambda(KillDelegate, TilePair.Key, Piece); 
				}
				else
				{
					AddDeathLambda(DeathDelegate, TilePair.Key, Piece);
					DyingPieces.Add(Piece);
				}
			}
		}
			
	}

	DeathDelegate.Broadcast();
	KillDelegate.Broadcast();
	for(auto Piece : DyingPieces)
	{
		Piece->MLC_DestroyPiece();
	}
}

void UCCStateResolve::AddKillLambda(FKillDeathDelegate& Delegate, ACCTile* Tile, ACCPieceBase* Piece)
{
	Delegate.AddLambda([this, Tile, Piece]()
	{
		GameState->GetEffectManager()->TriggerResolveEffect(
			false, Piece->CardDataRowHandle, Piece, TArray<ACCTile*>(), EEffectTriggerType::OnKill, Tile->GetPieces(), FIntPoint(), LastActions[Piece]);
	});
}

void UCCStateResolve::AddDeathLambda(FKillDeathDelegate& Delegate, ACCTile* Tile, ACCPieceBase* Piece)
{
	Delegate.AddLambda([this, Tile, Piece]()
	{
		GameState->GetEffectManager()->TriggerResolveEffect(
			false, Piece->CardDataRowHandle, Piece, TArray<ACCTile*>(), EEffectTriggerType::OnDeath, Tile->GetPieces(), FIntPoint(), FPlayerActionData());
	});
}
