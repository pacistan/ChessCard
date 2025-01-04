#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/CCGridManagerInterface.h"
#include "Interfaces/Clickable.h"
#include "Interfaces/Hoverable.h"
#include "Player/CCPlayerStart.h"

#include "CCTile.generated.h"

class ACCPieceBase;

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnClickTileDelegate, ACCTile*, Tile);

USTRUCT()
struct FTileContent
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FName UnitRowName;

	UPROPERTY()
	ETeam Team;

	UPROPERTY()
	int UnitId;
};

UENUM()
enum class ETileType : uint8
{Normal, Disabled, Player1, Player2, Player3, Player4, ScoreTile, BonusTile, Unit, Highlighted};

UENUM()
enum class EHighlightMode : uint8
{
	Normal UMETA(DisplayName = "Normal"),
	Path UMETA(DisplayName = "Path"),
	Effect UMETA(DisplayName = "Effect")
};

UCLASS()
class CHESSCARD_API ACCTile : public AActor, public IClickable, public IHoverable, public ICCGridManagerInterface
{
	GENERATED_BODY()

public:
	ACCTile();

	/* ------------------------------------------ MEMBERS -------------------------------------------*/
public:
	UPROPERTY()
	FOnClickTileDelegate OnClickEvent;
	
private:
	UPROPERTY(EditAnywhere, Category="", meta=(AllowPrivateAccess))
	TObjectPtr<UStaticMeshComponent> MeshComponent;
	
	UPROPERTY(VisibleAnywhere)
	TArray<TObjectPtr<ACCPieceBase>> Pieces;
	
	UPROPERTY()
	FTileContent CurrentTileContent;

	UPROPERTY(EditAnywhere)
	ETileType TileType;
	
	UPROPERTY(EditDefaultsOnly)
	TMap<ETileType, TObjectPtr<UMaterialInterface>> MaterialMap;

	UPROPERTY(EditDefaultsOnly)
	TMap<EHighlightMode, TObjectPtr<UMaterialInterface>> HighlightMaterial;

	UPROPERTY(VisibleAnywhere)
	EHighlightMode CurrentHighlightMode;
	
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UMaterialInterface> HoveredMaterial;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> BaseMaterial;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> NormalCrossTileHighlight;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> EffectCrossTileHighlight;

	UPROPERTY()
	ETileType PreviousTileType;

	UPROPERTY(VisibleAnywhere)
	int RowNum;

	UPROPERTY(VisibleAnywhere)
	int ColumnNum;

	UPROPERTY(VisibleAnywhere)
	bool IsHovered;

	UPROPERTY(VisibleAnywhere)
	bool IsHighlighted;

public:
	UPROPERTY()
	bool IsAboutToReceivePiece;	
	/* ------------------------------------------ EDITOR -------------------------------------------*/
	UFUNCTION(CallInEditor)
	void BlueprintEditorTick(float DeltaTime);

public:
	UFUNCTION()
	void UpdateMaterial();
	
	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
public:
	void SetHighlight(bool bIsHighlight, FOnClickTileDelegate OnClickDelegate, EHighlightMode HighlightMode = EHighlightMode::Normal);
	
	/* ------------------------------------------ INTERFACE -------------------------------------------*/
public:
	virtual void Click(ACCPlayerPawn* Player) override;
	virtual void StartHover(ACCPlayerPawn* Player) override;
	virtual void StopHover(ACCPlayerPawn* Player) override;
	
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
	
protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
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

	UFUNCTION()
	void AddPiece(ACCPieceBase* Piece);

	UFUNCTION(NetMulticast, Reliable)
	void MLC_AddPiece(ACCPieceBase* Piece);
	
	UFUNCTION()
	void RemovePiece(ACCPieceBase* Piece);

	UFUNCTION(NetMulticast, Reliable)
	void MLC_RemovePiece(ACCPieceBase* Piece);
	
	UFUNCTION()
	bool IsAccessibleForTeam(ETeam Team);

	UFUNCTION()
	bool ContainPiece(){return Pieces.Num() > 0;}

	UFUNCTION()
	TArray<ACCPieceBase*> GetPieces(){return Pieces;};

	UFUNCTION()
	ACCPieceBase* GetPiece(FGuid TargetID);

	UFUNCTION()
	bool GetIsHighlighted(){return IsHighlighted;}
};
