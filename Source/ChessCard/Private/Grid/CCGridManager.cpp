#include "Grid/CCGridManager.h"
#include "EngineUtils.h"
#include "IndexTypes.h"
#include "Card/FUnitMovementData.h"
#include "Grid/CCTile.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetStringLibrary.h"
#include "Macro/CCLogMacro.h"
#include "..\..\Public\TileActor\PatternMapEndPoint.h"

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

void ACCGridManager::GetTargetTiles(TArray<FUnitMovementData>& OutMovementData,
	TMap<FIntPoint, TArray<FPatternMapEndPoint>>& PatternSet)
{
	if(OutMovementData[0].MovementDirection == EMovementDirection::Diagonal)
	{
		TArray<FPatternMapEndPoint> UpRightArray;
		UpRightArray.Reserve(OutMovementData.Num());
		FPatternMapEndPoint UpRightDirectionData;
		UpRightDirectionData.Direction = FIntPoint(1, 1);
		UpRightDirectionData.MovementType = OutMovementData[0].MovementType;
		UpRightArray.Add(UpRightDirectionData);
		SimulateMovementOnGrid(PatternSet, UpRightArray, OutMovementData);

		TArray<FPatternMapEndPoint> DownRightArray;
		DownRightArray.Reserve(OutMovementData.Num());
		FPatternMapEndPoint DownRightDirectionData;
		DownRightDirectionData.Direction = FIntPoint(1, -1);
		DownRightDirectionData.MovementType = OutMovementData[0].MovementType;
		DownRightArray.Add(DownRightDirectionData);
		SimulateMovementOnGrid(PatternSet,  DownRightArray, OutMovementData);

		TArray<FPatternMapEndPoint> DownLeftArray;
		DownLeftArray.Reserve(OutMovementData.Num());
		FPatternMapEndPoint DownLeftDirectionData;
		DownLeftDirectionData.Direction = FIntPoint(-1, -1);
		DownLeftDirectionData.MovementType = OutMovementData[0].MovementType;
		DownLeftArray.Add(DownLeftDirectionData);
		SimulateMovementOnGrid(PatternSet, DownLeftArray, OutMovementData);
		
		TArray<FPatternMapEndPoint> UpLeftArray;
		UpLeftArray.Reserve(OutMovementData.Num());
		FPatternMapEndPoint UpLeftDirectionData;
		UpLeftDirectionData.Direction = FIntPoint(-1, 1);
		UpLeftDirectionData.MovementType = OutMovementData[0].MovementType;
		UpLeftArray.Add(UpLeftDirectionData);
		SimulateMovementOnGrid(PatternSet, UpLeftArray, OutMovementData);
	}
	else
	{
		TArray<FPatternMapEndPoint> UpArray;
		UpArray.Reserve(OutMovementData.Num());
		FPatternMapEndPoint UpDirectionData;
		UpDirectionData.Direction = FIntPoint(0, 1);
		UpDirectionData.MovementType = OutMovementData[0].MovementType;
		UpArray.Add(UpDirectionData);
		SimulateMovementOnGrid(PatternSet, UpArray, OutMovementData);

		TArray<FPatternMapEndPoint> RightArray;
		RightArray.Reserve(OutMovementData.Num());
		FPatternMapEndPoint RightDirectionData;
		RightDirectionData.Direction = FIntPoint(1, 0);
		RightDirectionData.MovementType = OutMovementData[0].MovementType;
		RightArray.Add(RightDirectionData);
		SimulateMovementOnGrid(PatternSet, RightArray, OutMovementData);

		TArray<FPatternMapEndPoint> DownArray;
		DownArray.Reserve(OutMovementData.Num());
		FPatternMapEndPoint DownDirectionData;
		DownDirectionData.Direction = FIntPoint(0, -1);
		DownDirectionData.MovementType = OutMovementData[0].MovementType;
		DownArray.Add(DownDirectionData);
		SimulateMovementOnGrid(PatternSet, DownArray, OutMovementData);

		TArray<FPatternMapEndPoint> LeftArray;
		LeftArray.Reserve(OutMovementData.Num());
		FPatternMapEndPoint LeftDirectionData;
		LeftDirectionData.Direction = FIntPoint(-1, 0);
		LeftDirectionData.MovementType = OutMovementData[0].MovementType;
		LeftArray.Add(LeftDirectionData);
		SimulateMovementOnGrid(PatternSet, LeftArray, OutMovementData);
	}
	
}

void ACCGridManager::SimulateMovementOnGrid(TMap<FIntPoint, TArray<FPatternMapEndPoint>>& PatternSet,
	TArray<FPatternMapEndPoint>& CurrentArray,TArray<FUnitMovementData>& MovementData)
{
	if(MovementData.Num() == CurrentArray.Num())
	{
		FPatternMapEndPoint PatternMapEndPoint;
		FIntPoint RelativeEndPosition = FIntPoint(0, 0);
		for(int i = 0; i < CurrentArray.Num(); i++)
		{
			RelativeEndPosition = FIntPoint(RelativeEndPosition.X + CurrentArray[i].X, RelativeEndPosition.Y + CurrentArray[i].Y);
		}
		PatternMapEndPoint.MovementType = MovementData.Last().MovementType;
		PatternMapEndPoint.Directions = CurrentArray;
		PatternSet.Add(RelativeEndPosition, PatternMapEndPoint);
		return;	
	}
	
	FPatternMapEndPoint LastDirectionData = CurrentArray.Last();
	int DirectionMagnitude = FMath::Abs(LastDirectionData.Direction.X) + FMath::Abs(LastDirectionData.Direction.Y);
	if(DirectionMagnitude == 2)
	{
		switch(MovementData[CurrentArray.Num()].MovementDirection)
		{
		case EMovementDirection::Up:
			{
				TArray<FPatternMapEndPoint> NewArray = CurrentArray;
				FPatternMapEndPoint UpDirectionData;
				UpDirectionData.Direction = FIntPoint(LastDirectionData.Direction.X, 0);
				UpDirectionData.MovementType = MovementData[CurrentArray.Num()].MovementType;
				NewArray.Add(UpDirectionData);
				SimulateMovementOnGrid(PatternSet, NewArray, MovementData);

				FPatternMapEndPoint UpDirectionData2;
				UpDirectionData2.Direction = FIntPoint(0, LastDirectionData.Direction.Y);
				UpDirectionData2.MovementType = MovementData[CurrentArray.Num()].MovementType;
				CurrentArray.Add(UpDirectionData2);
				SimulateMovementOnGrid(PatternSet, CurrentArray, MovementData);
			}
			break;
		case EMovementDirection::Side:
			{
				TArray<FIntPoint> NewArray = CurrentArray;
				NewArray.Add(FIntPoint(-LastDirectionData.X, 0));
				SimulateMovementOnGrid(PatternSet, NewArray, MovementData);

				CurrentArray.Add(FIntPoint(0, -LastDirectionData.Y));
				SimulateMovementOnGrid(PatternSet, CurrentArray, MovementData);
			}
			break;
		case EMovementDirection::Diagonal:
			{
				CurrentArray.Add(LastDirectionData);
				SimulateMovementOnGrid(PatternSet, CurrentArray, MovementData);
			}
			break;
		case EMovementDirection::Up_Side:
			{
				TArray<FIntPoint> NewArrayUp1 = CurrentArray;
				NewArrayUp1.Add(FIntPoint(LastDirectionData.X, 0));
				SimulateMovementOnGrid(PatternSet, NewArrayUp1, MovementData);

				TArray<FIntPoint> NewArrayUp2 = CurrentArray;
				NewArrayUp2.Add(FIntPoint(0, LastDirectionData.Y));
				SimulateMovementOnGrid(PatternSet, NewArrayUp2, MovementData);

				TArray<FIntPoint> SideArray1 = CurrentArray;
				SideArray1.Add(FIntPoint(-LastDirectionData.X, 0));
				SimulateMovementOnGrid(PatternSet, SideArray1, MovementData);

				CurrentArray.Add(FIntPoint(0, -LastDirectionData.Y));
				SimulateMovementOnGrid(PatternSet, CurrentArray, MovementData);
			}
			break;
		case EMovementDirection::Up_Diagonal:
			{
				TArray<FIntPoint> NewArrayUp1 = CurrentArray;
				NewArrayUp1.Add(FIntPoint(LastDirectionData.X, 0));
				SimulateMovementOnGrid(PatternSet, NewArrayUp1, MovementData);
	
				TArray<FIntPoint> NewArrayUp2 = CurrentArray;
				NewArrayUp2.Add(FIntPoint(0, LastDirectionData.Y));
				SimulateMovementOnGrid(PatternSet, NewArrayUp2, MovementData);
	
				CurrentArray.Add(LastDirectionData);
				SimulateMovementOnGrid(PatternSet, CurrentArray, MovementData);
			}
			break;
		case EMovementDirection::Up_Side_Diagonal:
			{
				TArray<FIntPoint> NewArrayUp1 = CurrentArray;
				NewArrayUp1.Add(FIntPoint(LastDirectionData.X, 0));
				SimulateMovementOnGrid(PatternSet, NewArrayUp1, MovementData);

				TArray<FIntPoint> NewArrayUp2 = CurrentArray;
				NewArrayUp2.Add(FIntPoint(0, LastDirectionData.Y));
				SimulateMovementOnGrid(PatternSet, NewArrayUp2, MovementData);

				TArray<FIntPoint> SideArray1 = CurrentArray;
				SideArray1.Add(FIntPoint(-LastDirectionData.X, 0));
				SimulateMovementOnGrid(PatternSet, SideArray1, MovementData);

				TArray<FIntPoint> SideArray2 = CurrentArray;
				SideArray2.Add(FIntPoint(0, -LastDirectionData.Y));
				SimulateMovementOnGrid(PatternSet, SideArray2, MovementData);
				
				
				CurrentArray.Add(FIntPoint(0, -LastDirectionData.Y));
				SimulateMovementOnGrid(PatternSet, CurrentArray, MovementData);
			}
			break;
		case EMovementDirection::Side_Diagonal:
			{
				TArray<FIntPoint> SideArray1 = CurrentArray;
				SideArray1.Add(FIntPoint(-LastDirectionData.X, 0));
				SimulateMovementOnGrid(PatternSet, SideArray1, MovementData);
	
				TArray<FIntPoint> SideArray2 = CurrentArray;
				SideArray2.Add(FIntPoint(0, -LastDirectionData.Y));
				SimulateMovementOnGrid(PatternSet, SideArray2, MovementData);
	
				CurrentArray.Add(FIntPoint(0, -LastDirectionData.Y));
				SimulateMovementOnGrid(PatternSet, CurrentArray, MovementData);
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
				CurrentArray.Add(LastDirectionData);
				SimulateMovementOnGrid(PatternSet, CurrentArray, MovementData);
			}
			break;
		case EMovementDirection::Side:
			{
				TArray<FIntPoint> NewArray = CurrentArray;
				NewArray.Add(FIntPoint(LastDirectionData.Y, -LastDirectionData.X));
				SimulateMovementOnGrid(PatternSet, NewArray, MovementData);

				CurrentArray.Add(FIntPoint(-LastDirectionData.Y, LastDirectionData.X));
				SimulateMovementOnGrid(PatternSet, CurrentArray, MovementData);
			}
			break;
		case EMovementDirection::Diagonal:
			{
				TArray<FIntPoint> NewArray = CurrentArray;
				NewArray.Add(FIntPoint(LastDirectionData.X + LastDirectionData.Y, LastDirectionData.Y - LastDirectionData.X));
				SimulateMovementOnGrid(PatternSet, NewArray, MovementData);

				CurrentArray.Add(FIntPoint(LastDirectionData.X - LastDirectionData.Y, LastDirectionData.Y + LastDirectionData.X));
				SimulateMovementOnGrid(PatternSet, CurrentArray, MovementData);
			}
			break;
		case EMovementDirection::Up_Side:
			{
				TArray<FIntPoint> ArraySide1 = CurrentArray;
				ArraySide1.Add(FIntPoint(LastDirectionData.Y, -LastDirectionData.X));
				SimulateMovementOnGrid(PatternSet, ArraySide1, MovementData);

				TArray<FIntPoint> ArraySide2 = CurrentArray;
				ArraySide2.Add(FIntPoint(-LastDirectionData.Y, LastDirectionData.X));
				SimulateMovementOnGrid(PatternSet, ArraySide2, MovementData);

				CurrentArray.Add(LastDirectionData);
				SimulateMovementOnGrid(PatternSet, CurrentArray, MovementData);
			}
			break;
		case EMovementDirection::Up_Diagonal:
			{
				TArray<FIntPoint> DiagonalArray1 = CurrentArray;
				DiagonalArray1.Add(FIntPoint(LastDirectionData.X + LastDirectionData.Y, LastDirectionData.Y - LastDirectionData.X));
				SimulateMovementOnGrid(PatternSet, DiagonalArray1, MovementData);

				TArray<FIntPoint> DiagonalArray2 = CurrentArray;
				DiagonalArray2.Add(FIntPoint(LastDirectionData.X - LastDirectionData.Y, LastDirectionData.Y + LastDirectionData.X));
				SimulateMovementOnGrid(PatternSet, DiagonalArray2, MovementData);

				CurrentArray.Add(LastDirectionData);
				SimulateMovementOnGrid(PatternSet, CurrentArray, MovementData);
			}
			break;
		case EMovementDirection::Up_Side_Diagonal:
			{
				TArray<FIntPoint> ArraySide1 = CurrentArray;
				ArraySide1.Add(FIntPoint(LastDirectionData.Y, -LastDirectionData.X));
				SimulateMovementOnGrid(PatternSet, ArraySide1, MovementData);

				TArray<FIntPoint> ArraySide2 = CurrentArray;
				ArraySide2.Add(FIntPoint(-LastDirectionData.Y, LastDirectionData.X));
				SimulateMovementOnGrid(PatternSet, ArraySide2, MovementData);
				
				TArray<FIntPoint> DiagonalArray1 = CurrentArray;
				DiagonalArray1.Add(FIntPoint(LastDirectionData.X + LastDirectionData.Y, LastDirectionData.Y - LastDirectionData.X));
				SimulateMovementOnGrid(PatternSet, DiagonalArray1, MovementData);

				TArray<FIntPoint> DiagonalArray2 = CurrentArray;
				DiagonalArray2.Add(FIntPoint(LastDirectionData.X - LastDirectionData.Y, LastDirectionData.Y + LastDirectionData.X));
				SimulateMovementOnGrid(PatternSet, DiagonalArray2, MovementData);

				CurrentArray.Add(LastDirectionData);
				SimulateMovementOnGrid(PatternSet, CurrentArray, MovementData);
			}
			break;
		case EMovementDirection::Side_Diagonal:
			{
				TArray<FIntPoint> ArraySide1 = CurrentArray;
				ArraySide1.Add(FIntPoint(LastDirectionData.Y, -LastDirectionData.X));
				SimulateMovementOnGrid(PatternSet, ArraySide1, MovementData);

				TArray<FIntPoint> ArraySide2 = CurrentArray;
				ArraySide2.Add(FIntPoint(-LastDirectionData.Y, LastDirectionData.X));
				SimulateMovementOnGrid(PatternSet, ArraySide2, MovementData);
					
				TArray<FIntPoint> DiagonalArray1 = CurrentArray;
				DiagonalArray1.Add(FIntPoint(LastDirectionData.X + LastDirectionData.Y, LastDirectionData.Y - LastDirectionData.X));
				SimulateMovementOnGrid(PatternSet, DiagonalArray1, MovementData);

				CurrentArray.Add(FIntPoint(LastDirectionData.X - LastDirectionData.Y, LastDirectionData.Y + LastDirectionData.X));
				SimulateMovementOnGrid(PatternSet, CurrentArray, MovementData);
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
	FOnClickTileDelegate OnClickTileDelegate;
	for(auto Tile : MappedGrid[ETileType::Highlighted])
	{
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
