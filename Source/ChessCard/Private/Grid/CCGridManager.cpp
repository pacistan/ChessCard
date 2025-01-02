#include "Grid/CCGridManager.h"
#include "EngineUtils.h"
#include "IndexTypes.h"
#include "Card/FUnitMovementData.h"
#include "Grid/CCTile.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
#include "Macro/CCLogMacro.h"
#include "..\..\Public\TileActor\PatternMapEndPoint.h"
#include "GameModes/CCGameMode.h"

ACCGridManager::ACCGridManager()
{
	PrimaryActorTick.bCanEverTick = false;
}

void ACCGridManager::GenerateGrid()
{
	TArray<AActor*> Tiles;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACCTile::StaticClass(), Tiles);
	for (const auto& tile : Tiles)
	{
		tile->Destroy();
	}

	// Clear the grid array
	Tiles.Empty();

	UE_LOG(LogTemp, Warning, TEXT("Initialization"));
	FActorSpawnParameters SpawnParams;
	for (size_t i = 0; i < GridSize; i++)
	{
		for (size_t j = 0; j < GridSize; j++)
		{
			FVector SpawnLocation = FVector( i * 100 * TileWidth + i * 100 * TileSpacingWidth, j * 100 * TileWidth + j * 100 * TileSpacingWidth, 0);
			ACCTile* SpawnedTile = GetWorld()->SpawnActor<ACCTile>(TilePrefab, SpawnLocation, FRotator::ZeroRotator, SpawnParams);
#if WITH_EDITOR
			SpawnedTile->SetActorLabel(FString::Printf(TEXT("Tile_%d_%d"), static_cast<int>(i), static_cast<int>(j)));
#endif
			SpawnedTile->AttachToActor(this, FAttachmentTransformRules::SnapToTargetIncludingScale);
			SpawnedTile->SetActorLocation(SpawnLocation);
			SpawnedTile->SetRowNum(i);
			SpawnedTile->SetColumnNum(j);
			SpawnedTile->SetActorScale3D(FVector(TileWidth, TileWidth, 1));
		}
	}
}

void ACCGridManager::UpdateTilesMaterials()
{
	TArray<AActor*> Actors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACCTile::StaticClass(), Actors);
	for(auto Actor : Actors)
	{
		ACCTile* Tile = Cast<ACCTile>(Actor);
		Tile->UpdateMaterial();
	}
}

FVector ACCGridManager::CoordinatesToPosition(FIntPoint Coordinates)
{
	return FVector(Coordinates.X * ((TileWidth + TileSpacingWidth) * 100), Coordinates.Y * ((TileWidth + TileSpacingWidth) * 100), 0);
}

void ACCGridManager::GetTargetTiles(TArray<FUnitMovementData>& OutMovementData,
	TArray<TArray<FPatternMapEndPoint>>& PatternList)
{
	TArray<FPatternMapEndPoint> BaseArray;
	BaseArray.Reserve(OutMovementData.Num());
	
	bool IsPotentialEnd;
	if(OutMovementData[0].MovementDirection == EMovementDirection::Diagonal)
	{
		auto UpRightDirectionData = GetArrayOfDirectionData(BaseArray, FIntPoint(1, 1), OutMovementData[0].MovementType, IsPotentialEnd);
		SimulateMovementOnGrid(PatternList, UpRightDirectionData, OutMovementData, IsPotentialEnd);

		auto DownRightDirectionData = GetArrayOfDirectionData(BaseArray, FIntPoint(1, -1), OutMovementData[0].MovementType, IsPotentialEnd);
		SimulateMovementOnGrid(PatternList, DownRightDirectionData, OutMovementData, IsPotentialEnd);
		
		auto DownLeftDirectionData = GetArrayOfDirectionData(BaseArray, FIntPoint(-1, -1), OutMovementData[0].MovementType, IsPotentialEnd);
		SimulateMovementOnGrid(PatternList, DownLeftDirectionData, OutMovementData, IsPotentialEnd);

		auto UpLeftDirectionData = GetArrayOfDirectionData(BaseArray, FIntPoint(-1, 1), OutMovementData[0].MovementType, IsPotentialEnd);
		SimulateMovementOnGrid(PatternList, UpLeftDirectionData, OutMovementData, IsPotentialEnd);
	}
	else
	{
		auto UpDirectionData = GetArrayOfDirectionData(BaseArray, FIntPoint(0, 1), OutMovementData[0].MovementType, IsPotentialEnd);
		SimulateMovementOnGrid(PatternList, UpDirectionData, OutMovementData, IsPotentialEnd);

		auto RightDirectionData = GetArrayOfDirectionData(BaseArray, FIntPoint(1, 0), OutMovementData[0].MovementType, IsPotentialEnd);
		SimulateMovementOnGrid(PatternList, RightDirectionData, OutMovementData, IsPotentialEnd);
		
		auto DownDirectionData = GetArrayOfDirectionData(BaseArray, FIntPoint(0, -1), OutMovementData[0].MovementType, IsPotentialEnd);
		SimulateMovementOnGrid(PatternList, DownDirectionData, OutMovementData, IsPotentialEnd);

		auto LeftDirectionData = GetArrayOfDirectionData(BaseArray, FIntPoint(-1, 0), OutMovementData[0].MovementType, IsPotentialEnd);
		SimulateMovementOnGrid(PatternList, LeftDirectionData, OutMovementData, IsPotentialEnd);

	}
}

void ACCGridManager::SimulateMovementOnGrid(TArray<TArray<FPatternMapEndPoint>>& PatternList,
	TArray<FPatternMapEndPoint>& CurrentArray,TArray<FUnitMovementData>& MovementData, bool IsPotentialEnd)
{
	// If All the movement data elements have been run through we stop the recursion and register the Movement Data with a key in the PatternMap.
	if(MovementData.Num() == CurrentArray.Num())
	{
		PatternList.Add(CurrentArray);
		return;	
	}
	else if(IsPotentialEnd)
	{
		PatternList.Add(CurrentArray);
	}
	
	FPatternMapEndPoint LastDirectionData = CurrentArray.Last();
	int DirectionMagnitude = FMath::Abs(LastDirectionData.Direction.X) + FMath::Abs(LastDirectionData.Direction.Y);
	if(DirectionMagnitude == 2)
	{
		switch(MovementData[CurrentArray.Num()].MovementDirection)
		{
			case EMovementDirection::Up:
				{
					auto UpArray1 = GetArrayOfDirectionData(CurrentArray, FIntPoint(LastDirectionData.Direction.X, 0), MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
					SimulateMovementOnGrid(PatternList, UpArray1, MovementData, IsPotentialEnd);
	
					auto UpArray2 = GetArrayOfDirectionData(CurrentArray, FIntPoint(0, LastDirectionData.Direction.Y), MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
					SimulateMovementOnGrid(PatternList, UpArray2, MovementData, IsPotentialEnd);
				}
				break;
			case EMovementDirection::Side:
			{
			    auto SideArray1 = GetArrayOfDirectionData(CurrentArray, FIntPoint(-LastDirectionData.Direction.X, 0), MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
			    SimulateMovementOnGrid(PatternList, SideArray1, MovementData, IsPotentialEnd);
			
			    auto SideArray2 = GetArrayOfDirectionData(CurrentArray, FIntPoint(0, -LastDirectionData.Direction.Y), MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
			    SimulateMovementOnGrid(PatternList, SideArray2, MovementData, IsPotentialEnd);
			}
			break;
			case EMovementDirection::Diagonal:
			{
			    auto DiagonalArray = GetArrayOfDirectionData(CurrentArray, LastDirectionData.Direction, MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
			    SimulateMovementOnGrid(PatternList, DiagonalArray, MovementData, IsPotentialEnd);
			}
			break;
			case EMovementDirection::Up_Side:
			{
			    auto UpArray1 = GetArrayOfDirectionData(CurrentArray, FIntPoint(LastDirectionData.Direction.X, 0), MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
			    SimulateMovementOnGrid(PatternList, UpArray1, MovementData, IsPotentialEnd);
			
			    auto UpArray2 = GetArrayOfDirectionData(CurrentArray, FIntPoint(0, LastDirectionData.Direction.Y), MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
			    SimulateMovementOnGrid(PatternList, UpArray2, MovementData, IsPotentialEnd);
			
			    auto SideArray1 = GetArrayOfDirectionData(CurrentArray, FIntPoint(-LastDirectionData.Direction.X, 0), MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
			    SimulateMovementOnGrid(PatternList, SideArray1, MovementData, IsPotentialEnd);
			
			    auto SideArray2 = GetArrayOfDirectionData(CurrentArray, FIntPoint(0, -LastDirectionData.Direction.Y), MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
			    SimulateMovementOnGrid(PatternList, SideArray2, MovementData, IsPotentialEnd);
			}
			break;
			case EMovementDirection::Up_Diagonal:
			{
			    auto UpArray1 = GetArrayOfDirectionData(CurrentArray, FIntPoint(LastDirectionData.Direction.X, 0), MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
			    SimulateMovementOnGrid(PatternList, UpArray1, MovementData, IsPotentialEnd);
			
			    auto UpArray2 = GetArrayOfDirectionData(CurrentArray, FIntPoint(0, LastDirectionData.Direction.Y), MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
			    SimulateMovementOnGrid(PatternList, UpArray2, MovementData, IsPotentialEnd);
			
			    auto DiagonalArray = GetArrayOfDirectionData(CurrentArray, LastDirectionData.Direction, MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
			    SimulateMovementOnGrid(PatternList, DiagonalArray, MovementData, IsPotentialEnd);
			}
			break;
			case EMovementDirection::Up_Side_Diagonal:
			{
			    auto UpArray1 = GetArrayOfDirectionData(CurrentArray, FIntPoint(LastDirectionData.Direction.X, 0), MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
			    SimulateMovementOnGrid(PatternList, UpArray1, MovementData, IsPotentialEnd);
			
			    auto UpArray2 = GetArrayOfDirectionData(CurrentArray, FIntPoint(0, LastDirectionData.Direction.Y), MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
			    SimulateMovementOnGrid(PatternList, UpArray2, MovementData, IsPotentialEnd);
			
			    auto SideArray1 = GetArrayOfDirectionData(CurrentArray, FIntPoint(-LastDirectionData.Direction.X, 0), MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
			    SimulateMovementOnGrid(PatternList, SideArray1, MovementData, IsPotentialEnd);
			
			    auto SideArray2 = GetArrayOfDirectionData(CurrentArray, FIntPoint(0, -LastDirectionData.Direction.Y), MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
			    SimulateMovementOnGrid(PatternList, SideArray2, MovementData, IsPotentialEnd);
			
			    auto DiagonalArray = GetArrayOfDirectionData(CurrentArray, LastDirectionData.Direction, MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
			    SimulateMovementOnGrid(PatternList, DiagonalArray, MovementData, IsPotentialEnd);
			}
			break;
			case EMovementDirection::Side_Diagonal:
			{
			    auto SideArray1 = GetArrayOfDirectionData(CurrentArray, FIntPoint(-LastDirectionData.Direction.X, 0), MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
			    SimulateMovementOnGrid(PatternList, SideArray1, MovementData, IsPotentialEnd);
			
			    auto SideArray2 = GetArrayOfDirectionData(CurrentArray, FIntPoint(0, -LastDirectionData.Direction.Y), MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
			    SimulateMovementOnGrid(PatternList, SideArray2, MovementData, IsPotentialEnd);
			
			    auto DiagonalArray = GetArrayOfDirectionData(CurrentArray, LastDirectionData.Direction, MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
			    SimulateMovementOnGrid(PatternList, DiagonalArray, MovementData, IsPotentialEnd);
			}
			break;
		}
	}
	else if(DirectionMagnitude == 1)
	{
	    switch(MovementData[CurrentArray.Num()].MovementDirection)
    	{
    	    case EMovementDirection::Up:
    	    {
    	        auto UpArray = GetArrayOfDirectionData(CurrentArray, LastDirectionData.Direction, MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
    	        SimulateMovementOnGrid(PatternList, UpArray, MovementData, IsPotentialEnd);
    	    }
    	    break;
    	    case EMovementDirection::Side:
    	    {
    	        auto SideArray1 = GetArrayOfDirectionData(CurrentArray, FIntPoint(LastDirectionData.Direction.Y, -LastDirectionData.Direction.X), MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
    	        SimulateMovementOnGrid(PatternList, SideArray1, MovementData, IsPotentialEnd);
	
    	        auto SideArray2 = GetArrayOfDirectionData(CurrentArray, FIntPoint(-LastDirectionData.Direction.Y, LastDirectionData.Direction.X), MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
    	        SimulateMovementOnGrid(PatternList, SideArray2, MovementData, IsPotentialEnd);
    	    }
    	    break;
    	    case EMovementDirection::Diagonal:
    	    {
    	        auto DiagonalArray1 = GetArrayOfDirectionData(CurrentArray, FIntPoint(LastDirectionData.Direction.X + LastDirectionData.Direction.Y, LastDirectionData.Direction.Y - LastDirectionData.Direction.X), MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
    	        SimulateMovementOnGrid(PatternList, DiagonalArray1, MovementData, IsPotentialEnd);
	
    	        auto DiagonalArray2 = GetArrayOfDirectionData(CurrentArray, FIntPoint(LastDirectionData.Direction.X - LastDirectionData.Direction.Y, LastDirectionData.Direction.Y + LastDirectionData.Direction.X), MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
    	        SimulateMovementOnGrid(PatternList, DiagonalArray2, MovementData, IsPotentialEnd);
    	    }
    	    break;
    	    case EMovementDirection::Up_Side:
    	    {
    	        auto UpArray = GetArrayOfDirectionData(CurrentArray, LastDirectionData.Direction, MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
    	        SimulateMovementOnGrid(PatternList, UpArray, MovementData, IsPotentialEnd);
	
    	        auto SideArray1 = GetArrayOfDirectionData(CurrentArray, FIntPoint(LastDirectionData.Direction.Y, -LastDirectionData.Direction.X), MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
    	        SimulateMovementOnGrid(PatternList, SideArray1, MovementData, IsPotentialEnd);
	
    	        auto SideArray2 = GetArrayOfDirectionData(CurrentArray, FIntPoint(-LastDirectionData.Direction.Y, LastDirectionData.Direction.X), MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
    	        SimulateMovementOnGrid(PatternList, SideArray2, MovementData, IsPotentialEnd);
    	    }
    	    break;
    	    case EMovementDirection::Up_Diagonal:
    	    {
    	        auto UpArray = GetArrayOfDirectionData(CurrentArray, LastDirectionData.Direction, MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
    	        SimulateMovementOnGrid(PatternList, UpArray, MovementData, IsPotentialEnd);
	
    	        auto DiagonalArray1 = GetArrayOfDirectionData(CurrentArray, FIntPoint(LastDirectionData.Direction.X + LastDirectionData.Direction.Y, LastDirectionData.Direction.Y - LastDirectionData.Direction.X), MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
    	        SimulateMovementOnGrid(PatternList, DiagonalArray1, MovementData, IsPotentialEnd);
	
    	        auto DiagonalArray2 = GetArrayOfDirectionData(CurrentArray, FIntPoint(LastDirectionData.Direction.X - LastDirectionData.Direction.Y, LastDirectionData.Direction.Y + LastDirectionData.Direction.X), MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
    	        SimulateMovementOnGrid(PatternList, DiagonalArray2, MovementData, IsPotentialEnd);
    	    }
    	    break;
    	    case EMovementDirection::Up_Side_Diagonal:
    	    {
    	        auto UpArray = GetArrayOfDirectionData(CurrentArray, LastDirectionData.Direction, MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
    	        SimulateMovementOnGrid(PatternList, UpArray, MovementData, IsPotentialEnd);
	
    	        auto SideArray1 = GetArrayOfDirectionData(CurrentArray, FIntPoint(LastDirectionData.Direction.Y, -LastDirectionData.Direction.X), MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
    	        SimulateMovementOnGrid(PatternList, SideArray1, MovementData, IsPotentialEnd);
	
    	        auto SideArray2 = GetArrayOfDirectionData(CurrentArray, FIntPoint(-LastDirectionData.Direction.Y, LastDirectionData.Direction.X), MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
    	        SimulateMovementOnGrid(PatternList, SideArray2, MovementData, IsPotentialEnd);
	
    	        auto DiagonalArray1 = GetArrayOfDirectionData(CurrentArray, FIntPoint(LastDirectionData.Direction.X + LastDirectionData.Direction.Y, LastDirectionData.Direction.Y - LastDirectionData.Direction.X), MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
    	        SimulateMovementOnGrid(PatternList, DiagonalArray1, MovementData, IsPotentialEnd);
	
    	        auto DiagonalArray2 = GetArrayOfDirectionData(CurrentArray, FIntPoint(LastDirectionData.Direction.X - LastDirectionData.Direction.Y, LastDirectionData.Direction.Y + LastDirectionData.Direction.X), MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
    	        SimulateMovementOnGrid(PatternList, DiagonalArray2, MovementData, IsPotentialEnd);
    	    }
    	    break;
    	    case EMovementDirection::Side_Diagonal:
    	    {
    	        auto SideArray1 = GetArrayOfDirectionData(CurrentArray, FIntPoint(LastDirectionData.Direction.Y, -LastDirectionData.Direction.X), MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
    	        SimulateMovementOnGrid(PatternList, SideArray1, MovementData, IsPotentialEnd);
	
    	        auto SideArray2 = GetArrayOfDirectionData(CurrentArray, FIntPoint(-LastDirectionData.Direction.Y, LastDirectionData.Direction.X), MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
    	        SimulateMovementOnGrid(PatternList, SideArray2, MovementData, IsPotentialEnd);
	
    	        auto DiagonalArray1 = GetArrayOfDirectionData(CurrentArray, FIntPoint(LastDirectionData.Direction.X + LastDirectionData.Direction.Y, LastDirectionData.Direction.Y - LastDirectionData.Direction.X), MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
    	        SimulateMovementOnGrid(PatternList, DiagonalArray1, MovementData, IsPotentialEnd);
	
    	        auto DiagonalArray2 = GetArrayOfDirectionData(CurrentArray, FIntPoint(LastDirectionData.Direction.X - LastDirectionData.Direction.Y, LastDirectionData.Direction.Y + LastDirectionData.Direction.X), MovementData[CurrentArray.Num()].MovementType, IsPotentialEnd);
    	        SimulateMovementOnGrid(PatternList, DiagonalArray2, MovementData, IsPotentialEnd);
    	    }
    	    break;
		}
	}
}

TArray<FPatternMapEndPoint> ACCGridManager::GetArrayOfDirectionData(TArray<FPatternMapEndPoint>& PreviousDirectionData,
	FIntPoint NewDirection, EMovementType InMovementType, bool& IsPotentialEnd)
{
	TArray<FPatternMapEndPoint> NewDirectionDataArray = PreviousDirectionData;
	FPatternMapEndPoint NewDirectionData;
	NewDirectionData.Direction = NewDirection;
	NewDirectionData.MovementType = InMovementType;
	NewDirectionDataArray.Add(NewDirectionData);
	IsPotentialEnd = InMovementType == EMovementType::Stoppable;
	return NewDirectionDataArray;
}

void ACCGridManager::ApplyLambdaToTileType(ETileType TileType, FTileTypeDelegate TileLambdaFunc)
{
	for(const auto& TileCoordinates : MappedGrid[TileType])
	{
		if(Grid.Num() > TileCoordinates.X && Grid[TileCoordinates.X].Num() > TileCoordinates.Y)
		{
			TileLambdaFunc.ExecuteIfBound(Grid[TileCoordinates.X][TileCoordinates.Y]);
		}
		else
		{
			FString EnumAsString = UEnum::GetValueAsString(TileType);
			DEBUG_WARNING("Coord : %i %i with Type : %s", TileCoordinates.X, TileCoordinates.Y, *EnumAsString);
			return;
		}
	}
}

void ACCGridManager::RegisterTileAsType(FIntPoint TileCoordinates, ETileType TileType)
{
	if(MappedGrid[TileType].Contains(TileCoordinates))
	{
		DEBUG_ERROR("Tile was already registered : %i %i", TileCoordinates.X, TileCoordinates.Y);
	}
	else
	{
		MappedGrid[TileType].Add(TileCoordinates);
	}
}

void ACCGridManager::UnregisterTileAsType(FIntPoint TileCoordinates, ETileType TileType)
{
	if(!MappedGrid[TileType].Contains(TileCoordinates))
	{
		DEBUG_ERROR("Tile was not registered but tried to be unregistered : %i %i", TileCoordinates.X, TileCoordinates.Y);
	}
	else
	{
		MappedGrid[TileType].Remove(TileCoordinates);
	}
}

void ACCGridManager::UnhighlightTiles()
{
	FOnClickTileDelegate OnClickTileDelegate;
	for(int i = MappedGrid[ETileType::Highlighted].Num() - 1; i >= 0; i--)
	{
		auto Tile = MappedGrid[ETileType::Highlighted][i];
		Grid[Tile.X][Tile.Y]->SetHighlight(false, OnClickTileDelegate);
	}
}

ACCTile* ACCGridManager::GetTile(FIntPoint Coordinates)
{
	if(Coordinates.X >= 0 && Coordinates.Y >= 0 && Coordinates.X < Grid.Num() && Coordinates.Y < Grid[0].Num())
	{
		return Grid[Coordinates.X][Coordinates.Y];
	}
	return nullptr;
}

void ACCGridManager::InitializeBonusTileMap()
{
	for(auto Tile : MappedGrid[ETileType::BonusTile])
	{
		GetWorld()->GetAuthGameMode<ACCGameMode>()->BonusTileMap.Add(GetTile(Tile), ETeam::None);
	}
}

void ACCGridManager::BeginPlay()
{
	Super::BeginPlay();
	
	TArray<ACCTile*> Tiles;
	Tiles.Reserve(GridSize * GridSize);
	for (TActorIterator<ACCTile> It(GetWorld()); It; ++It)
	{
		Tiles.Add(*It);
	}

	GridSize = FMath::RoundToInt32(FMath::Sqrt(static_cast<float>(Tiles.Num())));

	if(Tiles.Num() != GridSize * GridSize)
	{
		DEBUG_ERROR("Number of Tiles Found is wrong. Num = %i", Tiles.Num());
		return;
	}
	if (UEnum* EnumPtr = StaticEnum<ETileType>())
	{
		for (int32 i = 0; i < EnumPtr->NumEnums() - 1; i++)
		{
			ETileType TileTypeValue = static_cast<ETileType>(i);
			MappedGrid.Add(TileTypeValue);
		}
	}
	
	Grid.SetNum(GridSize);
	for(int i = 0; i < Grid.Num(); i++)
	{
		Grid[i].SetNum(GridSize);
	}

	for(const auto& Tile : Tiles)
	{
		Grid[Tile->GetRowNum()][Tile->GetColumnNum()] = Tile;
		MappedGrid[Tile->GetTileType()].Add(FIntPoint(Tile->GetRowNum(), Tile->GetColumnNum()));
	}

}
