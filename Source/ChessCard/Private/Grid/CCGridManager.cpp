#include "Grid/CCGridManager.h"
#include "EngineUtils.h"
#include "IndexTypes.h"
#include "Card/FUnitMovementData.h"
#include "Grid/CCTile.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
#include "Macro/CCLogMacro.h"

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

FVector ACCGridManager::CoordinatesToPosition(FIntPoint Coordinates)
{
	return FVector();
}

void ACCGridManager::GetTargetTiles(TArray<FUnitMovementData>& MovementData,
	TMap<FIntPoint, TArray<FIntPoint>>& PatternMap)
{
	if(MovementData[0].MovementDirection == EMovementDirection::Diagonal)
	{
		TArray<FIntPoint> UpRightArray;
		UpRightArray.Reserve(MovementData.Num());
		UpRightArray.Add(FIntPoint(1, 1));
		SimulateMovementOnGrid(PatternMap, UpRightArray, MovementData);

		TArray<FIntPoint> DownRightArray;
		DownRightArray.Reserve(MovementData.Num());
		DownRightArray.Add(FIntPoint( 1,  - 1));
		SimulateMovementOnGrid(PatternMap,  DownRightArray, MovementData);

		TArray<FIntPoint> DownLeftArray;
		DownLeftArray.Reserve(MovementData.Num());
		DownLeftArray.Add(FIntPoint(-1, -1));
		SimulateMovementOnGrid(PatternMap, DownLeftArray, MovementData);
		
		TArray<FIntPoint> UpLeftArray;
		UpLeftArray.Reserve(MovementData.Num());
		UpLeftArray.Add(FIntPoint(-1, 1));
		SimulateMovementOnGrid(PatternMap, UpLeftArray, MovementData);
	}
	else
	{
		TArray<FIntPoint> UpArray;
		UpArray.Reserve(MovementData.Num());
		UpArray.Add(FIntPoint(0, 1));
		SimulateMovementOnGrid(PatternMap, UpArray, MovementData);

		TArray<FIntPoint> RightArray;
		RightArray.Reserve(MovementData.Num());
		RightArray.Add(FIntPoint( 1,  0));
		SimulateMovementOnGrid(PatternMap,  RightArray, MovementData);

		TArray<FIntPoint> DownArray;
		DownArray.Reserve(MovementData.Num());
		DownArray.Add(FIntPoint(0, -1));
		SimulateMovementOnGrid(PatternMap, DownArray, MovementData);
		
		TArray<FIntPoint> LeftArray;
		LeftArray.Reserve(MovementData.Num());
		LeftArray.Add(FIntPoint(-1, 0));
		SimulateMovementOnGrid(PatternMap, LeftArray, MovementData);
	}
	
}

void ACCGridManager::SimulateMovementOnGrid(TMap<FIntPoint, TArray<FIntPoint>>& PatternMap,
	TArray<FIntPoint>& CurrentArray,TArray<FUnitMovementData>& MovementData)
{
	if(MovementData.Num() == CurrentArray.Num())
	{
		FIntPoint Key = FIntPoint(0, 0);
		for(int i = 0; i < CurrentArray.Num(); i++)
		{
			Key = FIntPoint(Key.X + CurrentArray[i].X, Key.Y + CurrentArray[i].Y);
		}
		PatternMap.Add(Key, CurrentArray);
		return;	
	}
	
	FIntPoint LastDirection = CurrentArray.Last();
	int DirectionMagnitude = FMath::Abs(LastDirection.X) + FMath::Abs(LastDirection.Y);
	if(DirectionMagnitude == 2)
	{
		switch(MovementData[CurrentArray.Num()].MovementDirection)
		{
		case EMovementDirection::Up:
			{
				TArray<FIntPoint> NewArray = CurrentArray;
				NewArray.Add(FIntPoint(LastDirection.X, 0));
				SimulateMovementOnGrid(PatternMap, NewArray, MovementData);

				CurrentArray.Add(FIntPoint(0, LastDirection.Y));
				SimulateMovementOnGrid(PatternMap, CurrentArray, MovementData);
			}
			break;
		case EMovementDirection::Side:
			{
				TArray<FIntPoint> NewArray = CurrentArray;
				NewArray.Add(FIntPoint(-LastDirection.X, 0));
				SimulateMovementOnGrid(PatternMap, NewArray, MovementData);

				CurrentArray.Add(FIntPoint(0, -LastDirection.Y));
				SimulateMovementOnGrid(PatternMap, CurrentArray, MovementData);
			}
			break;
		case EMovementDirection::Diagonal:
			{
				CurrentArray.Add(LastDirection);
				SimulateMovementOnGrid(PatternMap, CurrentArray, MovementData);
			}
			break;
		case EMovementDirection::Up_Side:
			{
				TArray<FIntPoint> NewArrayUp1 = CurrentArray;
				NewArrayUp1.Add(FIntPoint(LastDirection.X, 0));
				SimulateMovementOnGrid(PatternMap, NewArrayUp1, MovementData);

				TArray<FIntPoint> NewArrayUp2 = CurrentArray;
				NewArrayUp2.Add(FIntPoint(0, LastDirection.Y));
				SimulateMovementOnGrid(PatternMap, NewArrayUp2, MovementData);

				TArray<FIntPoint> SideArray1 = CurrentArray;
				SideArray1.Add(FIntPoint(-LastDirection.X, 0));
				SimulateMovementOnGrid(PatternMap, SideArray1, MovementData);

				CurrentArray.Add(FIntPoint(0, -LastDirection.Y));
				SimulateMovementOnGrid(PatternMap, CurrentArray, MovementData);
			}
			break;
		case EMovementDirection::Up_Diagonal:
			{
				TArray<FIntPoint> NewArrayUp1 = CurrentArray;
				NewArrayUp1.Add(FIntPoint(LastDirection.X, 0));
				SimulateMovementOnGrid(PatternMap, NewArrayUp1, MovementData);
	
				TArray<FIntPoint> NewArrayUp2 = CurrentArray;
				NewArrayUp2.Add(FIntPoint(0, LastDirection.Y));
				SimulateMovementOnGrid(PatternMap, NewArrayUp2, MovementData);
	
				CurrentArray.Add(LastDirection);
				SimulateMovementOnGrid(PatternMap, CurrentArray, MovementData);
			}
			break;
		case EMovementDirection::Up_Side_Diagonal:
			{
				TArray<FIntPoint> NewArrayUp1 = CurrentArray;
				NewArrayUp1.Add(FIntPoint(LastDirection.X, 0));
				SimulateMovementOnGrid(PatternMap, NewArrayUp1, MovementData);

				TArray<FIntPoint> NewArrayUp2 = CurrentArray;
				NewArrayUp2.Add(FIntPoint(0, LastDirection.Y));
				SimulateMovementOnGrid(PatternMap, NewArrayUp2, MovementData);

				TArray<FIntPoint> SideArray1 = CurrentArray;
				SideArray1.Add(FIntPoint(-LastDirection.X, 0));
				SimulateMovementOnGrid(PatternMap, SideArray1, MovementData);

				TArray<FIntPoint> SideArray2 = CurrentArray;
				SideArray2.Add(FIntPoint(0, -LastDirection.Y));
				SimulateMovementOnGrid(PatternMap, SideArray2, MovementData);
				
				
				CurrentArray.Add(FIntPoint(0, -LastDirection.Y));
				SimulateMovementOnGrid(PatternMap, CurrentArray, MovementData);
			}
			break;
		case EMovementDirection::Side_Diagonal:
			{
				TArray<FIntPoint> SideArray1 = CurrentArray;
				SideArray1.Add(FIntPoint(-LastDirection.X, 0));
				SimulateMovementOnGrid(PatternMap, SideArray1, MovementData);
	
				TArray<FIntPoint> SideArray2 = CurrentArray;
				SideArray2.Add(FIntPoint(0, -LastDirection.Y));
				SimulateMovementOnGrid(PatternMap, SideArray2, MovementData);
	
				CurrentArray.Add(FIntPoint(0, -LastDirection.Y));
				SimulateMovementOnGrid(PatternMap, CurrentArray, MovementData);
			}
			break;
		}
	}
	else
	{
		switch(MovementData[CurrentArray.Num()].MovementDirection)
		{
		case EMovementDirection::Up:
			{
				CurrentArray.Add(LastDirection);
				SimulateMovementOnGrid(PatternMap, CurrentArray, MovementData);
			}
			break;
		case EMovementDirection::Side:
			{
				TArray<FIntPoint> NewArray = CurrentArray;
				NewArray.Add(FIntPoint(LastDirection.Y, -LastDirection.X));
				SimulateMovementOnGrid(PatternMap, NewArray, MovementData);

				CurrentArray.Add(FIntPoint(-LastDirection.Y, LastDirection.X));
				SimulateMovementOnGrid(PatternMap, CurrentArray, MovementData);
			}
			break;
		case EMovementDirection::Diagonal:
			{
				TArray<FIntPoint> NewArray = CurrentArray;
				NewArray.Add(FIntPoint(LastDirection.X + LastDirection.Y, LastDirection.Y - LastDirection.X));
				SimulateMovementOnGrid(PatternMap, NewArray, MovementData);

				CurrentArray.Add(FIntPoint(LastDirection.X - LastDirection.Y, LastDirection.Y + LastDirection.X));
				SimulateMovementOnGrid(PatternMap, CurrentArray, MovementData);
			}
			break;
		case EMovementDirection::Up_Side:
			{
				TArray<FIntPoint> ArraySide1 = CurrentArray;
				ArraySide1.Add(FIntPoint(LastDirection.Y, -LastDirection.X));
				SimulateMovementOnGrid(PatternMap, ArraySide1, MovementData);

				TArray<FIntPoint> ArraySide2 = CurrentArray;
				ArraySide2.Add(FIntPoint(-LastDirection.Y, LastDirection.X));
				SimulateMovementOnGrid(PatternMap, ArraySide2, MovementData);

				CurrentArray.Add(LastDirection);
				SimulateMovementOnGrid(PatternMap, CurrentArray, MovementData);
			}
			break;
		case EMovementDirection::Up_Diagonal:
			{
				TArray<FIntPoint> DiagonalArray1 = CurrentArray;
				DiagonalArray1.Add(FIntPoint(LastDirection.X + LastDirection.Y, LastDirection.Y - LastDirection.X));
				SimulateMovementOnGrid(PatternMap, DiagonalArray1, MovementData);

				TArray<FIntPoint> DiagonalArray2 = CurrentArray;
				DiagonalArray2.Add(FIntPoint(LastDirection.X - LastDirection.Y, LastDirection.Y + LastDirection.X));
				SimulateMovementOnGrid(PatternMap, DiagonalArray2, MovementData);

				CurrentArray.Add(LastDirection);
				SimulateMovementOnGrid(PatternMap, CurrentArray, MovementData);
			}
			break;
		case EMovementDirection::Up_Side_Diagonal:
			{
				TArray<FIntPoint> ArraySide1 = CurrentArray;
				ArraySide1.Add(FIntPoint(LastDirection.Y, -LastDirection.X));
				SimulateMovementOnGrid(PatternMap, ArraySide1, MovementData);

				TArray<FIntPoint> ArraySide2 = CurrentArray;
				ArraySide2.Add(FIntPoint(-LastDirection.Y, LastDirection.X));
				SimulateMovementOnGrid(PatternMap, ArraySide2, MovementData);
				
				TArray<FIntPoint> DiagonalArray1 = CurrentArray;
				DiagonalArray1.Add(FIntPoint(LastDirection.X + LastDirection.Y, LastDirection.Y - LastDirection.X));
				SimulateMovementOnGrid(PatternMap, DiagonalArray1, MovementData);

				TArray<FIntPoint> DiagonalArray2 = CurrentArray;
				DiagonalArray2.Add(FIntPoint(LastDirection.X - LastDirection.Y, LastDirection.Y + LastDirection.X));
				SimulateMovementOnGrid(PatternMap, DiagonalArray2, MovementData);

				CurrentArray.Add(LastDirection);
				SimulateMovementOnGrid(PatternMap, CurrentArray, MovementData);
			}
			break;
		case EMovementDirection::Side_Diagonal:
			{
				TArray<FIntPoint> ArraySide1 = CurrentArray;
				ArraySide1.Add(FIntPoint(LastDirection.Y, -LastDirection.X));
				SimulateMovementOnGrid(PatternMap, ArraySide1, MovementData);

				TArray<FIntPoint> ArraySide2 = CurrentArray;
				ArraySide2.Add(FIntPoint(-LastDirection.Y, LastDirection.X));
				SimulateMovementOnGrid(PatternMap, ArraySide2, MovementData);
					
				TArray<FIntPoint> DiagonalArray1 = CurrentArray;
				DiagonalArray1.Add(FIntPoint(LastDirection.X + LastDirection.Y, LastDirection.Y - LastDirection.X));
				SimulateMovementOnGrid(PatternMap, DiagonalArray1, MovementData);

				CurrentArray.Add(FIntPoint(LastDirection.X - LastDirection.Y, LastDirection.Y + LastDirection.X));
				SimulateMovementOnGrid(PatternMap, CurrentArray, MovementData);
			}
			break;
		}
	}
}

void ACCGridManager::ApplyLambdaToTileType(ETileType TileType, const FTileTypeDelegate TileLambdaFunc)
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
	for(auto Tile : MappedGrid[ETileType::Highlighted])
	{
		Grid[Tile.X][Tile.Y]->SetHighlight(false);
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
