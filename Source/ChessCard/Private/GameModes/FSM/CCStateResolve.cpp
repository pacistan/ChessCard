#include "GameModes/FSM/CCStateResolve.h"

#include "GameModes/CCEffectManagerComponent.h"
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
	
	//Give Score to Players
	{
		FTileTypeDelegate TileTypeDelegate;
		auto Lambda = [this](ACCTile* Tile)
		{
			for(const auto& Unit : Tile->GetPieces())
			{
				//TODO : Add Score to Team
				//GameState->AddScore(Unit->GetTeam());
			}	
		};
		TileTypeDelegate.BindLambda(Lambda);
		GameState->GetGridManager()->ApplyLambdaToTileType(ETileType::ScoreTile,TileTypeDelegate);
	}

	//Increment Divine Anger Counter
	{
		FTileTypeDelegate TileTypeDelegate;
		auto Lambda = [](ACCTile* Tile)
		{
			for(const auto& Piece : Tile->GetPieces())
			{
				if(ACCTileUnit* Unit = Cast<ACCTileUnit>(Piece))
				{
					Unit->DivineAngerCounter++;
				}
			}	
		};
		TileTypeDelegate.BindLambda(Lambda);
		GameState->GetGridManager()->ApplyLambdaToTileType(ETileType::Unit, TileTypeDelegate);
	}
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
	
	for(auto TilePair : KillerTiles)
	{
		TilePair.Key->IsAboutToReceivePiece = false;
	}
	KillerTiles.Empty();
	SlaughterTiles.Empty();
	
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
				
				TargetTile->MLC_RemovePiece(Unit);
				FIntPoint EndMovementCoordinates = Unit->CurrentCoordinates;
				for(auto MvtData : Action.MovementData) EndMovementCoordinates += MvtData.Direction; 
				ACCTile* EndMovementTile = GameState->GetGridManager()->GetTile(EndMovementCoordinates);
				check(EndMovementTile);
				Unit->CurrentCoordinates = EndMovementCoordinates;
				EndMovementTile->MLC_AddPiece(Unit);
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
			TargetTile->IsAboutToReceivePiece = true;
			if(KillerTiles.Contains(TargetTile))
			{
				KillerTiles[TargetTile].Add(Unit);
				SlaughterTiles.FindOrAdd(TargetTile);
			}
			else
			{
				KillerTiles.Add(TargetTile, TArray<ACCPieceBase*>{Unit});
			}
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
		BloodSlaughterAndDeath();
		
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

		//Movement Proc
		ACCTileUnit* Unit = Cast<ACCTileUnit>(LastPiece);
		FIntPoint Coordinate = Unit->CurrentCoordinates;
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
			LastPiece, EffectTiles, EEffectTriggerType::OnMove, TArray<ACCPieceBase*>(), Direction);
		Unit->DivineAngerCounter = 0;
	}
}

DECLARE_MULTICAST_DELEGATE(FKillDeathDelegate);

void UCCStateResolve::BloodSlaughterAndDeath()
{
	FKillDeathDelegate KillDelegate;
	FKillDeathDelegate DeathDelegate;
	for(auto TilePair : KillerTiles)
	{
		if(SlaughterTiles.Contains(TilePair.Key))
		{
			for(auto Piece : TilePair.Key->GetPieces())
			{
				//Kill And Death
				if(TilePair.Value.Contains(Piece))
				{
					auto KillLambda = [this, TilePair, Piece]()
					{
						GameState->GetEffectManager()->TriggerResolveEffect(
							false, Piece->CardDataRowHandle, Piece, TArray<ACCTile*>(), EEffectTriggerType::OnKill, TilePair.Value, FIntPoint());
					};
					KillDelegate.AddLambda(KillLambda);
					auto DeathLambda = [this, TilePair, Piece]()
					{
						GameState->GetEffectManager()->TriggerResolveEffect(
							false, Piece->CardDataRowHandle, Piece, TArray<ACCTile*>(), EEffectTriggerType::OnDeath, TilePair.Value, FIntPoint());
					};
					DeathDelegate.AddLambda(DeathLambda);
				}
				//Death
				else
				{
					auto DeathLambda = [this, TilePair, Piece]()
					{
						GameState->GetEffectManager()->TriggerResolveEffect(
							false, Piece->CardDataRowHandle, Piece, TArray<ACCTile*>(), EEffectTriggerType::OnDeath, TilePair.Value, FIntPoint());
					};
					DeathDelegate.AddLambda(DeathLambda);
				}
			}
		}
		else
		{
			for(auto Piece : TilePair.Key->GetPieces())
			{
				//Kill
				if(TilePair.Value.Contains(Piece))
				{
					auto KillLambda = [this, TilePair, Piece]()
					{
						GameState->GetEffectManager()->TriggerResolveEffect(
							false, Piece->CardDataRowHandle, Piece, TArray<ACCTile*>(), EEffectTriggerType::OnKill, TilePair.Value, FIntPoint());
					};
				}
				//Death
				else
				{
					auto DeathLambda = [this, TilePair, Piece]()
					{
						GameState->GetEffectManager()->TriggerResolveEffect(
							false, Piece->CardDataRowHandle, Piece, TArray<ACCTile*>(), EEffectTriggerType::OnDeath, TilePair.Value, FIntPoint());
					};
					DeathDelegate.AddLambda(DeathLambda);
				}
			}
		}
	}

	DeathDelegate.Broadcast();
	KillDelegate.Broadcast();
}
