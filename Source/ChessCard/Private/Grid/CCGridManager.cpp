#include "Grid/CCGridManager.h"
#include "EngineUtils.h"
#include "IndexTypes.h"
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



void ACCGridManager::ApplyLambdaToTileType(ETileType TileType, const FTileTypeDelegate TileLambdaFunc)
{
	for(const auto& TileCoordinates : MappedGrid[TileType])
	{
		if(Grid.Num() > TileCoordinates.A && Grid[TileCoordinates.A].Num() > TileCoordinates.B)
		{
			TileLambdaFunc.ExecuteIfBound(Grid[TileCoordinates.A][TileCoordinates.B]);
		}
		else
		{
			FString EnumAsString = UEnum::GetValueAsString(TileType);
			DEBUG_WARNING("Coord : %i %i with Type : %s", TileCoordinates.A, TileCoordinates.B, *EnumAsString);
			return;
		}
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
		MappedGrid[Tile->GetTileType()].Add(UE::Geometry::FIndex2i(Tile->GetRowNum(), Tile->GetColumnNum()));
	}
}
