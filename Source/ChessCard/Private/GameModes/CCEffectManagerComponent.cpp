#include "GameModes/CCEffectManagerComponent.h"
#include "Card/FCardData.h"
#include "GameModes/CCGameMode.h"
#include "GameModes/CCGameState.h"
#include "Grid/CCTile.h"
#include "Player/CCPlayerPawn.h"
#include "Player/CCPlayerState.h"
#include "TileActor/CCTileUnit.h"


UCCEffectManagerComponent::UCCEffectManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UCCEffectManagerComponent::TriggerResolveEffect(bool IsDivineAnger, FDataTableRowHandle InRowHandle, ACCPieceBase* InPiece,
	TArray<ACCTile*> InEffectTiles, EEffectTriggerType InTriggerType, TArray<ACCPieceBase*> RelevantUnits, FIntPoint Direction, FPlayerActionData ActionData)
{
	FCardData CardData = *InRowHandle.GetRow<FCardData>(TEXT(""));
	ACCGameState* GameState = GetOwner<ACCGameState>();
	ACCGameMode* GameMode = GetWorld()->GetAuthGameMode<ACCGameMode>();
	switch(CardData.EffectType)
	{
	case EEffectType::Midas:
		if(InTriggerType == EEffectTriggerType::OnDeath)
		{
			for(auto Unit : RelevantUnits)
			{
				ACCPlayerPawn* TargetPlayer = GameState->GetPlayerStateOfTeam(Unit->GetTeam())->GetPawn<ACCPlayerPawn>();
				TargetPlayer->RPC_AddCardToDeck(GoldRowHandle);
			}
		}
		else if(InTriggerType == EEffectTriggerType::OnKill)
		{
			auto PlayerPawns = GameMode->GetPlayerPawns();
			for(auto Player : PlayerPawns)
			{
				if(Player->GetPlayerState<ACCPlayerState>()->GetTeam() != InPiece->GetTeam())
				{
					Player->RPC_AddCardToDeck(GoldRowHandle);
				}
			}
		}
		break;
	case EEffectType::Promethee:
		if(InTriggerType == EEffectTriggerType::OnDeath)
		{
			ACCTile* SpawnTile = GameState->GetGridManager()->GetValidRandomAdjacentTile(GameState->GetGridManager()->GetTile(InPiece->CurrentCoordinates));
			const FRotator UnitRotation;
			const FVector UnitPosition = SpawnTile->GetActorLocation() + FVector::UpVector * 20;
			FActorSpawnParameters UnitSpawnParams;
			UnitSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			UnitSpawnParams.bNoFail = true;
			
			ACCPieceBase* Unit = GetWorld()->SpawnActor<ACCPieceBase>(GetWorld()->GetGameState<ACCGameState>()->PieceClass, UnitPosition, UnitRotation, UnitSpawnParams);
			Unit->InitUnit(FInitilizationProperties(FIntPoint(SpawnTile->GetRowNum(), SpawnTile->GetColumnNum()), InPiece->GetTeam(), FGuid::NewGuid(), EmbrasementRowHandle));
		}
		else if(InTriggerType == EEffectTriggerType::OnMove && IsDivineAnger)
		{
			if(InEffectTiles.IsEmpty())
			{
				return;
			}
			auto Pieces = InEffectTiles[0]->GetPieces();
			for(int k = Pieces.Num() - 1; k >= 0; k--)
			{
				TriggerResolveEffect(false, Pieces[k]->CardDataRowHandle, Pieces[k],TArray<ACCTile*>(),
					EEffectTriggerType::OnDeath, TArray<ACCPieceBase*>{InPiece}, FIntPoint(), FPlayerActionData());
				Pieces[k]->MLC_DestroyPiece();
			}
			
			const FRotator UnitRotation;
			const FVector UnitPosition = InEffectTiles[0]->GetActorLocation() + FVector::UpVector * 20;
			FActorSpawnParameters UnitSpawnParams;
			UnitSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			UnitSpawnParams.bNoFail = true;
			
			ACCPieceBase* Unit = GetWorld()->SpawnActor<ACCPieceBase>(GetWorld()->GetGameState<ACCGameState>()->PieceClass, UnitPosition, UnitRotation, UnitSpawnParams);
			Unit->InitUnit(FInitilizationProperties(FIntPoint(InEffectTiles[0]->GetRowNum(), InEffectTiles[0]->GetColumnNum()), InPiece->GetTeam(), FGuid::NewGuid(), EmbrasementRowHandle));
		}
		break;
	case EEffectType::Minotaur:
		if(InTriggerType == EEffectTriggerType::OnMove && IsDivineAnger)
		{
			DEBUG_WARNING("MINOTAUR EFFECT");
			for(auto Tile : InEffectTiles)
			{
				auto Pieces = Tile->GetPieces();
				for(int i = Pieces.Num() - 1; i >= 0; i--)
				{
					TriggerResolveEffect(false, Pieces[i]->CardDataRowHandle, Pieces[i], TArray<ACCTile*>(),
						EEffectTriggerType::OnDeath, TArray<ACCPieceBase*>{InPiece}, FIntPoint(), FPlayerActionData());
					Pieces[i]->MLC_DestroyPiece();
				}
			}
		}
		break;
	case EEffectType::Trojan:
		if(InTriggerType == EEffectTriggerType::OnDeath)
		{
			ACCTile* SpawnTile = GameState->GetGridManager()->GetValidRandomAdjacentTile(GameState->GetGridManager()->GetTile(InPiece->CurrentCoordinates));
			const FRotator UnitRotation;
			const FVector UnitPosition = SpawnTile->GetActorLocation() + FVector::UpVector * 20;
			FActorSpawnParameters UnitSpawnParams;
			UnitSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			UnitSpawnParams.bNoFail = true;
			
			ACCPieceBase* Unit = GetWorld()->SpawnActor<ACCPieceBase>(GetWorld()->GetGameState<ACCGameState>()->PieceClass, UnitPosition, UnitRotation, UnitSpawnParams);
			TArray<FName> RowNames = CardDataTable->GetRowNames();
			FDataTableRowHandle RandomRowHandle;
			RandomRowHandle.DataTable = CardDataTable;
			int RowNameIndex = FMath::RandRange(0, RowNames.Num() - 1);
			FCardData LinkedData;
			do
			{
				RandomRowHandle.RowName = FName(RowNames[RowNameIndex]);
				LinkedData = *RandomRowHandle.GetRow<FCardData>("");
				RowNameIndex = (RowNameIndex + 1) % RowNames.Num();
			}
			while(!LinkedData.IsRandomSpawnable);
			Unit->InitUnit(FInitilizationProperties(FIntPoint(SpawnTile->GetRowNum(), SpawnTile->GetColumnNum()), InPiece->GetTeam(), FGuid::NewGuid(), RandomRowHandle));
		}
		break;
	case EEffectType::Meduse:
		if(InTriggerType == EEffectTriggerType::OnMove)
		{
			for(auto Tile : InEffectTiles)
			{
				if(!Tile->GetPieces().IsEmpty())
				{
					for(auto Piece : Tile->GetPieces())
					{
						if(ACCTileUnit* Unit = Cast<ACCTileUnit>(Piece))
						{
							Unit->IsStunned = true;						
						}
					}
					break;
				}
			}
			if(IsDivineAnger)
			{
				for(int i = -1; i <= 1; i++)
				{
					for(int j = -1; j <= 1; j++)
					{
						int X = InPiece->CurrentCoordinates.X + i;
						int Y = InPiece->CurrentCoordinates.Y + j;
						ACCTile* Tile = GameState->GetGridManager()->GetTile(FIntPoint(X, Y));
						if(IsValid(Tile))
						{
							for(auto Piece : Tile->GetPieces())
							{
								if(ACCTileUnit* Unit = Cast<ACCTileUnit>(Piece))
								{
									Unit->IsStunned = true;
								}
							}
						}
					}
				}
			}
		}
		break;
	case EEffectType::Persee:
		if(InTriggerType == EEffectTriggerType::OnKill && !ActionData.MovementData.IsEmpty())
		{
			auto Unit = Cast<ACCTileUnit>(InPiece);
			FIntPoint NewDestination = Unit->CurrentCoordinates;
			for(auto Mvt : ActionData.MovementData) NewDestination += Mvt.Direction;
			if(GameState->GetGridManager()->GetTile(NewDestination))
			{
				for(auto PlayerState : GameState->PlayerArray)
				{
					auto CCPlayerState = Cast<ACCPlayerState>(PlayerState);
					if(CCPlayerState->GetTeam()	== Unit->Team)
					{
						FPlayerActionData NewActionData =ActionData;
						NewActionData.TargetCoord = NewDestination;
						GameMode->PlayerActions[CCPlayerState].Insert(NewActionData, 0);
					}
					else
					{
						FPlayerActionData NewActionData = FPlayerActionData();
						NewActionData.TargetCoord = FIntPoint(-1, -1);
						GameMode->PlayerActions[CCPlayerState].Insert(NewActionData, 0);
					}
				}
			}
		}
		break;
	case EEffectType::Circe:
		if(InTriggerType == EEffectTriggerType::OnKill)
		{
			
		}
		if(InTriggerType == EEffectTriggerType::OnMove && IsDivineAnger)
		{
			for(auto Tile : InEffectTiles)
			{
				if(!Tile->GetPieces().IsEmpty())
				{
					GameState->GetGridManager()->GetTile(InPiece->CurrentCoordinates)->MLC_RemovePiece(InPiece);
					ACCPieceBase* Piece = Tile->GetPieces()[0];
					FInitilizationProperties InitProperties = Piece->InitilizationProperties;
					InitProperties.Team = InPiece->Team;
					InitProperties.InstanceID = InPiece->UnitGuid;
					TriggerResolveEffect(false, Piece->CardDataRowHandle, Piece,TArray<ACCTile*>(),
							EEffectTriggerType::OnDeath, TArray<ACCPieceBase*>{InPiece}, FIntPoint(), FPlayerActionData());
					Piece->MLC_DestroyPiece();
					InPiece->InitUnit(InitProperties);
					break;
				}
			}
		}
		break;
	case EEffectType::Heracles:
		if(InTriggerType == EEffectTriggerType::OnMove && IsDivineAnger)
		{
			for(int i = -1; i <= 1; i++)
			{
				for(int j = -1; j <= 1; j++)
				{
					int X = InPiece->CurrentCoordinates.X + i;
					int Y = InPiece->CurrentCoordinates.Y + j;
					ACCTile* Tile = GameState->GetGridManager()->GetTile(FIntPoint(X, Y));
					if(IsValid(Tile))
					{
						auto Pieces = Tile->GetPieces();
						for(int k = Pieces.Num() - 1; k >= 0; k--)
						{
							TriggerResolveEffect(false, Pieces[k]->CardDataRowHandle, Pieces[k],TArray<ACCTile*>(),
								EEffectTriggerType::OnDeath, TArray<ACCPieceBase*>{InPiece}, FIntPoint(), FPlayerActionData());
							Pieces[i]->MLC_DestroyPiece();
						}
					}
				}
			}
		}
		break;
	case EEffectType::Embrasement:
		if(InTriggerType == EEffectTriggerType::OnMove)
		{
			for(int i = -1; i <= 1; i++)
			{
				for(int j = -1; j <= 1; j++)
				{
					int X = InPiece->CurrentCoordinates.X + i;
					int Y = InPiece->CurrentCoordinates.Y + j;
					ACCTile* Tile = GameState->GetGridManager()->GetTile(FIntPoint(X, Y));
					if(IsValid(Tile))
					{
						auto Pieces = Tile->GetPieces();
						for(int k = Pieces.Num() - 1; k >= 0; k--)
						{
							TriggerResolveEffect(false, Pieces[k]->CardDataRowHandle, Pieces[k],TArray<ACCTile*>(),
								EEffectTriggerType::OnDeath, TArray<ACCPieceBase*>{InPiece}, FIntPoint(), FPlayerActionData());
							Pieces[i]->MLC_DestroyPiece();
						}
					}
				}
			}
		}
		break;
	}
}

