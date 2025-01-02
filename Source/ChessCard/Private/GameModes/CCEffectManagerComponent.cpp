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

void UCCEffectManagerComponent::TriggerResolveEffect(FDataTableRowHandle InRowHandle, ACCPieceBase* InPiece,
	ACCTile* InEffectTiles, EEffectTriggerType InTriggerType, ACCPieceBase* RelevantUnit, FIntPoint Direction)
{
	FCardData CardData = *InRowHandle.GetRow<FCardData>(TEXT(""));
	ACCGameState* GameState = GetOwner<ACCGameState>();
	ACCGameMode* GameMode = GetWorld()->GetAuthGameMode<ACCGameMode>();
	switch(CardData.EffectType)
	{
	case EEffectType::Midas:
		if(InTriggerType == EEffectTriggerType::OnDeath)
		{
			ACCPlayerPawn* TargetPlayer = GameState->GetPlayerStateOfTeam(RelevantUnit->GetTeam())->GetPawn<ACCPlayerPawn>();
			TargetPlayer->RPC_AddCardToDeck(GoldRowHandle);
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
		else if(InTriggerType == EEffectTriggerType::DivineAnger)
		{
			const FRotator UnitRotation;
			const FVector UnitPosition = InEffectTiles->GetActorLocation() + FVector::UpVector * 20;
			FActorSpawnParameters UnitSpawnParams;
			UnitSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			UnitSpawnParams.bNoFail = true;
			
			ACCPieceBase* Unit = GetWorld()->SpawnActor<ACCPieceBase>(GetWorld()->GetGameState<ACCGameState>()->PieceClass, UnitPosition, UnitRotation, UnitSpawnParams);
			Unit->InitUnit(FInitilizationProperties(FIntPoint(InEffectTiles->GetRowNum(), InEffectTiles->GetColumnNum()), InPiece->GetTeam(), FGuid::NewGuid(), EmbrasementRowHandle));
		}
		break;
	case EEffectType::Minotaur:
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
		//GameState->GetGridManager
		//GameMode->
		break;
	case EEffectType::Circe:
		break;
	case EEffectType::Heracles:
		break;
	case EEffectType::Embrasement:
		break;
	}
}

