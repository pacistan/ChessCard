#include "Grid/CCGridManager.h"
#include "EngineUtils.h"
#include "IndexTypes.h"
#include "Grid/CCTile.h"
#include "Kismet/GameplayStatics.h"

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
			SpawnedTile->SetColumnNum(i);
			SpawnedTile->SetActorScale3D(FVector(TileWidth, TileWidth, 1));
		}
	}
}

void ACCGridManager::ApplyLambdaToTileType(ETileType TileType, const FTileTypeDelegate TileLambdaFunc)
{
	for(const auto& TileCoordinates : MappedGrid[TileType])
	{
			TileLambdaFunc.ExecuteIfBound(Grid[TileCoordinates.A][TileCoordinates.B]);
		/*if(Grid.Contains(TileCoordinates.A) && Grid[TileCoordinates.A].Contains(TileCoordinates.B))
		{
		}
		else
		{
			UE_LOG(LogTemp, Error, TEXT("Coord : %i %i"), TileCoordinates.A, TileCoordinates.B);
			return;
		}*/
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
		UE_LOG(LogTemp, Error, TEXT("Number of Tiles Found is wrong"));
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
	for(auto& GridCell : Grid)
	{
		GridCell.SetNum(GridSize);
	}

	for(const auto& Tile : Tiles)
	{
		Grid[Tile->GetRowNum()][Tile->GetColumnNum()] = Tile;
		MappedGrid[Tile->GetTileType()].Add(UE::Geometry::FIndex2i(Tile->GetRowNum(), Tile->GetColumnNum()));
	}
}
