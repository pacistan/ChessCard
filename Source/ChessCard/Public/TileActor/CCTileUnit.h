#pragma once

#include "CoreMinimal.h"
#include "CCPieceBase.h"
#include "Interfaces/CCGridManagerInterface.h"
#include "Interfaces/Clickable.h"
#include "Interfaces/Hoverable.h"
#include "CCTileUnit.generated.h"


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

	UPROPERTY(EditDefaultsOnly)
	FColor HighlightColor;

	UPROPERTY()
	FLinearColor BaseColor;

	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> MaterialInstance;
	
	//Map that contains all possible pattern movements. Key is relative end position
	//of movement value is array of directions to get to key
	TMap<FIntPoint, TArray<FIntPoint>> PatternMap;

	UPROPERTY()
	bool IsHighlighted;

	UPROPERTY()
	bool IsSelected;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	TArray<FUnitMovementData> Pattern;

	UPROPERTY()
	FIntPoint CurrentCoordinates;
	
	FOnClickUnitDelegate OnClickUnitEvent;
	FOnHoverUnitDelegate OnHoverUnitEvent;

	
	/* ----------------------------------------- FUNCTIONS -------------------------------------------*/
public:
	ACCTileUnit(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void SetHighlight(bool bToHighlight, FOnClickUnitDelegate InOnClickDelegate = FOnClickUnitDelegate(), FOnHoverUnitDelegate InOnHoverUnitDelegate = FOnHoverUnitDelegate());

	UFUNCTION()
	void HighlightDestinationTiles();

	UFUNCTION()
	void Select(ACCPlayerPawn* Player);

	UFUNCTION()
	void UnSelect();

	UFUNCTION()
	void SetTargetMap();
	
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/

	virtual void BeginPlay() override;
	virtual void StartHover(ACCPlayerPawn* Player) override;
	virtual void StopHover(ACCPlayerPawn* Player) override;
	virtual void Click(ACCPlayerPawn* Player) override;
};
