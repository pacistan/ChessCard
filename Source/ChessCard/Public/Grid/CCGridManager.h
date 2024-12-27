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

	void GetTargetTiles(TArray<FUnitMovementData>& OutMovementData,
	                    TMap<FIntPoint, TArray<FPatternMapEndPoint>>& PatternSet);
	
	void SimulateMovementOnGrid(TMap<FIntPoint, TArray<FPatternMapEndPoint>>& PatternSet,
	                            TArray<FPatternMapEndPoint>& CurrentArray, TArray<FUnitMovementData>& MovementData);

	void ApplyLambdaToTileType(ETileType TileType,const FTileTypeDelegate TileLambdaFunc);

	void RegisterTileAsType(FIntPoint TileCoordinates, ETileType TileType);

	void UnregisterTileAsType(FIntPoint TileCoordinates, ETileType TileType);

	UFUNCTION()
	void UnhighlightTiles();

	ACCTile* GetTile(FIntPoint Coordinates);

	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
protected:
	virtual void BeginPlay() override;
};
