#pragma once

#include "CoreMinimal.h"
#include "IndexTypes.h"
#include "GameFramework/Actor.h"
#include "CCGridManager.generated.h"

enum class EMovementType : uint8;
struct FUnitMovementData;
struct FPatternMapEndPoint;
enum class ETileType : uint8;
class ACCTile;




DECLARE_DELEGATE_OneParam(FTileTypeDelegate, ACCTile*);

UCLASS()
class CHESSCARD_API ACCGridManager : public AActor
{
	GENERATED_BODY()

public:
	ACCGridManager();

	/* ------------------------------------------ MEMBERS -------------------------------------------*/
private:
	UPROPERTY(EditAnywhere, meta = (ClampMin=0))
	int GridSize;

	UPROPERTY(EditAnywhere, meta = (ClampMin=0))
	float TileWidth;

	UPROPERTY(EditAnywhere, meta = (ClampMin=0))
	float TileSpacingWidth;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<ACCTile> TilePrefab;

	TArray<TArray<TObjectPtr<ACCTile>>> Grid;

	TMap<ETileType, TArray<FIntPoint>> MappedGrid;
	
	/* ------------------------------------------ EDITOR -------------------------------------------*/
	UFUNCTION(CallInEditor)
	void GenerateGrid();

	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
public:
	static FVector CoordinatesToPosition(FIntPoint Coordinates);

	// On Unit Spawn we create the Pattern Map. We want to get all possible path the unit can make when moving.
	// We want to store those paths for Movement and highlighting.
	// This process would be the same for every instance of the same unit type. It could perhaps be moved out of the game loop.
	void GetTargetTiles(TArray<FUnitMovementData>& OutMovementData,
								TMap<FIntPoint, TArray<FPatternMapEndPoint>>& PatternMap);

	// Recursively build elements of the Pattern Map. Keys are the end point of the potential movement 
	void SimulateMovementOnGrid(TMap<FIntPoint, TArray<FPatternMapEndPoint>>& PatternMap,
	                            TArray<FPatternMapEndPoint>& CurrentArray, TArray<FUnitMovementData>& MovementData, bool IsPotentialEnd);

	// Return an element of the PatternMap from its data. IsPotentialEnd will be true if the next element is stoppable.
	// If IsPotentialEnd is true the next iteration will create an endpoint for one of the elements of the PatternMap.
	TArray<FPatternMapEndPoint> GetArrayOfDirectionData(TArray<FPatternMapEndPoint>& PreviousDirectionData,
	                                                    FIntPoint NewDirection, EMovementType InMovementType, bool& IsPotentialEnd);

	// Will apply delegate to every Tile of the given type.
	void ApplyLambdaToTileType(ETileType TileType, FTileTypeDelegate TileLambdaFunc);

	UFUNCTION()
	void RegisterTileAsType(FIntPoint TileCoordinates, ETileType TileType);

	UFUNCTION()		
	void UnregisterTileAsType(FIntPoint TileCoordinates, ETileType TileType);

	UFUNCTION()
	void UnhighlightTiles();

	UFUNCTION()
	ACCTile* GetTile(FIntPoint Coordinates);

	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
protected:
	virtual void BeginPlay() override;
};
