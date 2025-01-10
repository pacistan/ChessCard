#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/CCGridManagerInterface.h"
#include "Interfaces/Selectable.h"
#include "TileActor/PatternMapEndPoint.h"
#include "CCCard.generated.h"


class ACCTile;
class UCCCardMovementComponent;
class ACCPieceBase;
class ACCTileUnit;
class UCCCardMovementComponent;
struct FCardData;

UENUM(BlueprintType)
enum class ECardState : uint8
{
	Inactive,
	Hovered,
	Selected,
	Played
};

USTRUCT(BlueprintType)
struct FMaterialArrayWrapper
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	TArray<UMaterialInterface*> Materials;
};

UCLASS(HideCategories(Collision, Input, HLOD, Physics, Events, Level_Instance, Cooking, World_Partition, Data_Layers,  Actor_Tick))
class CHESSCARD_API ACCCard : public AActor, public ISelectable, public IHoverable, public ICCGridManagerInterface
{
	GENERATED_BODY()

public:
	ACCCard();

	/* ------------------------------------------ MEMBERS -------------------------------------------*/
	UPROPERTY(EditAnywhere, Category="", BlueprintReadOnly, meta=(AllowPrivateAccess))
	TObjectPtr<UCCCardMovementComponent> CardMovement;

	//UPROPERTY(EditAnywhere, Category="", meta=(AllowPrivateAccess))
	//TObjectPtr<UStaticMeshComponent> CardMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite , Category="", meta=(AllowPrivateAccess))
	TObjectPtr<USceneComponent> CCRootComponent;
	
	FGuid CardUniqueID;
	
	UPROPERTY(VisibleAnywhere, Category="", BlueprintReadWrite)
	ECardState CurrentCardState;
	
	UPROPERTY()
	int32 CardIndex;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TMap<ECardState, FMaterialArrayWrapper> MaterialMap;

	UPROPERTY()
	bool IsHovered;

	UPROPERTY(Blueprintable, BlueprintReadOnly)
	FDataTableRowHandle CardRowHandle;

	UPROPERTY()
	TObjectPtr<ACCPlayerPawn> OwningPawn;

	UPROPERTY()
	bool IsCore = true;

	UPROPERTY()
	bool IsFleeting = false;
	
	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
private:

	UFUNCTION()
	void OnSelectCardEffects(bool bIsSelected, ACCPlayerPawn* Pawn);

	virtual void BeginPlay() override;
	
public:
	UFUNCTION(BlueprintImplementableEvent)
	void UpdateMaterials();
	
	UFUNCTION(BlueprintCallable)
	void Play(ACCPlayerPawn* Pawn);

	UFUNCTION()
	void Initialize();
	
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/

	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
protected:
	UFUNCTION()
	void SpawnLocalUnit(ACCTile* Tile);

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void CreateCardVisuals(FCardData DataTableRowHandle);
	
public:
	UFUNCTION()
	void MoveUnit(ACCTile* Tile, TArray<FPatternMapEndPoint> MovementData, TArray<AActor*>& MovementVisualActors, ACCTileUnit* Unit);

	UFUNCTION()
	void TriggerEmbrasement(ACCTile* Tile, TArray<AActor*>& MovementVisualActors, ACCTileUnit* Unit);

	/* ------------------------------------------ INTERFACE -------------------------------------------*/

public:
	virtual void StartHover(ACCPlayerPawn* Pawn) override;
	
	virtual void StopHover(ACCPlayerPawn* Pawn) override;

	virtual bool GetCardData(FDataTableRowHandle& OutRowHandle, bool& outHasDivineCount, int& outCount) override;
	
	void Unplay(ACCPlayerPawn* Pawn);

	virtual void Select(ACCPlayerPawn* Pawn) override;
	
	virtual void UnSelect(ACCPlayerPawn* Pawn) override;

	/* ------------------------------------------ GETTER/SETTER -------------------------------------------*/
public:
	UFUNCTION(BlueprintSetter)
	void SetCardIndex(int32 InCardIndex) { CardIndex = InCardIndex;}

	UFUNCTION(BlueprintGetter)
	FDataTableRowHandle GetDataTableRow(){return CardRowHandle;}

	UFUNCTION(BlueprintGetter)
	ECardState GetCurrentCardState(){return CurrentCardState;}

	UFUNCTION(BlueprintSetter)
	void SetDataTableRow(FDataTableRowHandle InRowHandle){CardRowHandle = InRowHandle;}

	UFUNCTION()
	void SetOwningPawn(ACCPlayerPawn* InOwningPawn){OwningPawn = InOwningPawn;}

	UFUNCTION(BlueprintImplementableEvent)
	void BPE_OnDiscardCard();
	
	UFUNCTION(BlueprintImplementableEvent)
	void BPE_OnDrawCard();

	UFUNCTION(BlueprintImplementableEvent)
	void BPE_OnPlay(bool IsMovement, int PlayIndex);

	UFUNCTION(BlueprintImplementableEvent)
	void BPE_Hover(bool ToHover);

	UFUNCTION(BlueprintImplementableEvent)
	void BPE_Select(bool ToSelect);

	UFUNCTION(BlueprintImplementableEvent)
	void BPE_ConstructCard(FDataTableRowHandle RowHandle);

	UFUNCTION(BlueprintImplementableEvent)
	void BPE_OnUnplay();
};
