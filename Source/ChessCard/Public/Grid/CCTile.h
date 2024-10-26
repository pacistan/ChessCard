#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/Clickable.h"
#include "Interfaces/Hoverable.h"
#include "CCTile.generated.h"

USTRUCT()
struct FTileContent
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FName UnitRowName;

	UPROPERTY()
	int PlayerId;

	UPROPERTY()
	int UnitId;
};

UENUM()
enum class ETileType : uint8
{Normal, Disabled, Player1, Player2, Player3, Player4, ScoreTile, BonusTile};

UCLASS()
class CHESSCARD_API ACCTile : public AActor, public IClickable, public IHoverable
{
	GENERATED_BODY()

public:
	ACCTile();

	/* ------------------------------------------ MEMBERS -------------------------------------------*/
private:
	UPROPERTY()
	FTileContent CurrentTileContent;

	UPROPERTY(EditAnywhere)
	ETileType TileType;

	UPROPERTY(EditDefaultsOnly)
	TMap<ETileType, FColor> ColorMap;

	UPROPERTY(EditDefaultsOnly)
	FColor HighlightColor;

	UPROPERTY(EditDefaultsOnly)
	FColor HoveredColor;
	
	UPROPERTY(EditAnywhere, Category="", meta=(AllowPrivateAccess))
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY()
	ETileType PreviousTileType;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MaterialInstance;
	
	UPROPERTY()
	int RowNum;

	UPROPERTY()
	int ColumnNum;

	UPROPERTY()
	bool IsHovered;

	UPROPERTY()
	bool IsHighlighted;
	
	/* ------------------------------------------ EDITOR -------------------------------------------*/
	UFUNCTION(CallInEditor)
	void BlueprintEditorTick(float DeltaTime);
	
	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
public:
	void SetHighlight(bool bIsHighlight);
	
	/* ------------------------------------------ INTERFACE -------------------------------------------*/
private:
	virtual void Click(ACCPlayerPawn* Player) override;
	virtual void StartHover(ACCPlayerPawn* Player) override;
	virtual void StopHover(ACCPlayerPawn* Player) override;
	
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
	
protected:
	virtual void BeginPlay() override;

	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	virtual bool ShouldTickIfViewportsOnly() const override;

	/* ------------------------------------------ GETTER/SETTER -------------------------------------------*/
public:
	UFUNCTION(BlueprintGetter)
	int GetRowNum()const {return RowNum;}
	
	UFUNCTION(BlueprintGetter)
    int GetColumnNum()const {return ColumnNum;} 

	UFUNCTION(BlueprintSetter)
	void SetRowNum(int InRowNum){ RowNum = InRowNum;}
	
	UFUNCTION(BlueprintSetter)
	void SetColumnNum(int InColumnNum){ ColumnNum = InColumnNum;}

	UFUNCTION(BlueprintGetter)
	ETileType GetTileType()const {return TileType;}
};
