#pragma once

#include "CoreMinimal.h"
#include "CCPieceBase.h"
#include "Grid/CCGridManager.h"
#include "Interfaces/CCGridManagerInterface.h"
#include "Interfaces/Clickable.h"
#include "Interfaces/Hoverable.h"
#include "PatternMapEndPoint.h"
#include "CCTileUnit.generated.h"


enum class ETeam : uint8;
DECLARE_DYNAMIC_DELEGATE(FOnHoverUnitDelegate);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnClickUnitDelegate, ACCPlayerPawn*, Player);

class UCCUnitMovementComponent;

/**
 *  Tile Actor that Have a the ability to move 
 */
UCLASS()
class CHESSCARD_API ACCTileUnit : public ACCPieceBase, public IHoverable, public IClickable, public ICCGridManagerInterface
{
	GENERATED_BODY()

	/* ------------------------------------------ MEMBERS -------------------------------------------*/
public:
	UPROPERTY(EditAnywhere, Category="", meta=(AllowPrivateAccess))
	TObjectPtr<UCCUnitMovementComponent> MovementComponent;


	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> HighlightMat;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> SelectedMaterial;
	
	UPROPERTY()
	TObjectPtr<UMaterialInterface> BaseMaterial;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MaterialInstance;
	
	//Map that contains all possible pattern movements. Key is relative end position
	//of movement value is array of directions to get to key
	TMap<FIntPoint, TArray<FPatternMapEndPoint>> PatternMap;

	UPROPERTY()
	bool IsHighlighted;

	UPROPERTY()
	bool IsSelected;

	UPROPERTY()
	bool IsMoved;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	TArray<FUnitMovementData> Pattern;

	UPROPERTY()
	FIntPoint CurrentCoordinates;
	
	UPROPERTY()
	ETeam Team;
	
	FOnClickUnitDelegate OnClickUnitEvent;
	FOnHoverUnitDelegate OnHoverUnitEvent;

	
	/* ----------------------------------------- FUNCTIONS -------------------------------------------*/
public:
	ACCTileUnit(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void SetHighlight(bool bToHighlight, FOnClickUnitDelegate InOnClickDelegate = FOnClickUnitDelegate(), FOnHoverUnitDelegate InOnHoverUnitDelegate = FOnHoverUnitDelegate());

	UFUNCTION()
	void HighlightDestinationTiles(ACCPlayerPawn* Pawn);

	UFUNCTION()
	void UnSelect();

	UFUNCTION()
	void SetTargetMap();

	UFUNCTION()
	ETeam GetTeam() {return Team;};

	UFUNCTION()
	void SetTeam(ETeam InTeam) {Team = InTeam;}

	UFUNCTION()
	bool GetIsMoved(){return IsMoved;}

	UFUNCTION()
	void SetIsMoved(bool InIsMoved){IsMoved = InIsMoved;}
	
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/

	virtual void BeginPlay() override;
	virtual void StartHover(ACCPlayerPawn* Player) override;
	virtual void StopHover(ACCPlayerPawn* Player) override;
	virtual void Click(ACCPlayerPawn* Player) override;
};
