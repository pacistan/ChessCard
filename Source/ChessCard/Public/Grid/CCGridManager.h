#pragma once

#include "CoreMinimal.h"
#include "IndexTypes.h"
#include "GameFramework/Actor.h"
#include "CCGridManager.generated.h"

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

	TMap<ETileType, TArray<UE::Geometry::FIndex2i>> MappedGrid;
	
	/* ------------------------------------------ EDITOR -------------------------------------------*/
	UFUNCTION(CallInEditor)
	void GenerateGrid();
	
	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
public:
	void ApplyLambdaToTileType(ETileType TileType,const FTileTypeDelegate TileLambdaFunc);

	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
protected:
	virtual void BeginPlay() override;
};
