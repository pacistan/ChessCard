#pragma once

#include "CoreMinimal.h"
#include "CCPieceBase.h"
#include "Grid/CCGridManager.h"
#include "Interfaces/CCGridManagerInterface.h"
#include "Interfaces/Clickable.h"
#include "Interfaces/Hoverable.h"
#include "PatternMapEndPoint.h"
#include "Card/CCCard.h"
#include "CCTileUnit.generated.h"

enum class ETeam : uint8;
DECLARE_DYNAMIC_DELEGATE(FOnHoverUnitDelegate);
DECLARE_DYNAMIC_DELEGATE_OneParam(FOnClickUnitDelegate, ACCPlayerPawn*, Player);

class UCCUnitMovementComponent;

/**
 *  Tile Actor that Have a the ability to move 
 */
UCLASS()
class CHESSCARD_API ACCTileUnit : public ACCPieceBase, public IHoverable, public IClickable
{
	GENERATED_BODY()

	/* ------------------------------------------ MEMBERS -------------------------------------------*/
public:
	UPROPERTY(EditAnywhere, Category="", meta=(AllowPrivateAccess))
	TObjectPtr<UCCUnitMovementComponent> MovementComponent;
	
	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> HighlightMat;

	UPROPERTY()
	TObjectPtr<UMaterialInterface> BaseMaterial;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UMaterialInterface> SelectedMaterial;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "CC|Runtime")
	TArray<FUnitMovementData> MovementDatas;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "CC|Runtime")
	TArray<FUnitMovementData> DivineAngerMovementDatas;
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "CC|Runtime", Replicated)
	int DivineAngerCounter;
	
	//Map that contains all possible pattern movements. Key is relative end position
	//of movement value is array of directions to get to key
	TArray<TArray<FPatternMapEndPoint>> PatternList;

	TArray<TArray<FPatternMapEndPoint>> DivineAngerPatternList;

	UPROPERTY()
	bool IsHighlighted;

	UPROPERTY(Replicated)
	bool IsMoved;

	
	UPROPERTY()
	TObjectPtr<ACCCard> LinkedCard;

	UPROPERTY()
	FUnitMovementData MovementData;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> MovementPointActorClass;

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> DestinationPointActorClass;
	
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AActor> EffectPointActorClass;
	
	FOnClickUnitDelegate OnClickUnitEvent;
	FOnHoverUnitDelegate OnHoverUnitEvent;

private:
	UPROPERTY(VisibleInstanceOnly, Replicated)
	bool IsStunned;

	/* ----------------------------------------- FUNCTIONS -------------------------------------------*/
public:
	ACCTileUnit(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void SetHighlight(bool bToHighlight, ETeam InTeam, FOnClickUnitDelegate InOnClickDelegate = FOnClickUnitDelegate(), FOnHoverUnitDelegate
	                  InOnHoverUnitDelegate = FOnHoverUnitDelegate());

	UFUNCTION()
	void HighlightDestinationTiles(ACCPlayerPawn* Pawn);
	void MinotaurHighlightDestinationTiles();

	UFUNCTION()
	void OnDestinationTileClicked(ACCTile* Tile);

	UFUNCTION()
	void SetTargetMap();

	UFUNCTION()
	bool GetIsMoved(){return IsMoved;}

	UFUNCTION()
	void SetIsMoved(bool InIsMoved){IsMoved = InIsMoved;}

	UFUNCTION()
	FIntPoint GetTravelRelativeCoordinates(TArray<FPatternMapEndPoint>& PatternMovement);

	UFUNCTION()
	bool GetIsStunned(){return IsStunned;}

	UFUNCTION()
	void SetIsStunned(bool InIsStunned, bool IsServerCall = true)
	{
		if(InIsStunned)
		{
			BPE_OnStunned(IsServerCall);
		}
		IsStunned = InIsStunned;
	}
	
	UFUNCTION(BlueprintImplementableEvent)
	void BPE_OnStunned(bool IsServerCall);
	
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/

	virtual void BeginPlay() override;
	virtual void StartHover(ACCPlayerPawn* Player) override;
	virtual void StopHover(ACCPlayerPawn* Player) override;
	virtual void Click(ACCPlayerPawn* Player) override;
	virtual void InternalInit() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void UnSelect() override;

};
