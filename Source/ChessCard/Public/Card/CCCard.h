#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/CCGridManagerInterface.h"
#include "Interfaces/Selectable.h"
#include "TileActor/CCTileUnit.h"
#include "Card/FCardData.h"
#include "CCCard.generated.h"


class ACCTile;
class UCCCardMovementComponent;
class ACCPieceBase;

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

UCLASS(HideCategories(Rendering, Collision, Actor, Input, HLOD, Physics, Events, Level_Instance, Cooking, World_Partition, Data_Layers,  Actor_Tick))
class CHESSCARD_API ACCCard : public AActor, public ISelectable, public IHoverable, public ICCGridManagerInterface
{
	GENERATED_BODY()

public:
	ACCCard();

	/* ------------------------------------------ MEMBERS -------------------------------------------*/
	UPROPERTY(EditAnywhere, Category="", BlueprintReadOnly, meta=(AllowPrivateAccess))
	TObjectPtr<UCCCardMovementComponent> CardMovement;

	UPROPERTY(EditAnywhere, Category="", meta=(AllowPrivateAccess))
	TObjectPtr<UStaticMeshComponent> CardMesh;

	UPROPERTY(VisibleAnywhere, Category="")
	ECardState CurrentCardState;

	UPROPERTY(EditAnywhere)
	TSubclassOf<ACCPieceBase> PieceClass;
	
	UPROPERTY()
	int32 CardIndex;

	UPROPERTY(EditDefaultsOnly)
	TMap<ECardState, FMaterialArrayWrapper> MaterialMap;

	UPROPERTY()
	bool IsHovered;

	UPROPERTY()
	FDataTableRowHandle CardRowHandle;

	UPROPERTY()
	TObjectPtr<ACCPlayerPawn> OwningPawn;
	
	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
private:
	UFUNCTION()
	void UpdateMaterials();

	UFUNCTION()
	void OnSelectCardEffects(bool bIsSelected, ACCPlayerPawn* Pawn);

	virtual void BeginPlay() override;
	
public:
	UFUNCTION(BlueprintCallable)
	void Play(ACCPlayerPawn* Pawn);
	
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/

	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
protected:
	UFUNCTION()
	void SpawnUnit(ACCTile* Tile);

public:
	UFUNCTION()
	void MoveUnit(ACCTile* Unit);

	/* ------------------------------------------ INTERFACE -------------------------------------------*/

public:
	virtual void StartHover(ACCPlayerPawn* Pawn) override;
	
	virtual void StopHover(ACCPlayerPawn* Pawn) override;
	
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
};
