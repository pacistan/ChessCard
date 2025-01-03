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
	TArray<ACCTile*> InEffectTiles, EEffectTriggerType InTriggerType, TArray<ACCPieceBase*> RelevantUnits, FIntPoint Direction)
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
		else if(InTriggerType == EEffectTriggerType::OnMove)
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
			if(!IsValid(InEffectTiles[0]))
			{
				return;
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
		for(auto Tile : InEffectTiles)
		{
			for(auto Piece : Tile->GetPieces())
			{
				TriggerResolveEffect(false, Piece->CardDataRowHandle, Piece, TArray<ACCTile*>(), EEffectTriggerType::OnDeath, TArray<ACCPieceBase*>{InPiece}, FIntPoint());
				Piece->MLC_DestroyPiece();
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
		break;
	case EEffectType::Persee:
		break;
	case EEffectType::Circe:
		break;
	case EEffectType::Heracles:
		break;
	case EEffectType::Embrasement:
		break;
	}
}

