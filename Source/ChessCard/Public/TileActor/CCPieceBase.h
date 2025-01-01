#pragma once

#include "CoreMinimal.h"
#include "Card/FCardData.h"
#include "GameFramework/Actor.h"
#include "Card/FUnitMovementData.h"
#include "Interfaces/CCGridManagerInterface.h"
#include "CCPieceBase.generated.h"

class ACCPlayerState;
class ACCPlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTileEffectTriggered, ACCPieceBase* , Owner);

/**
 *  Base class for all the Actor that will be placed on the board
 */
UCLASS()
class CHESSCARD_API ACCPieceBase : public AActor, public ICCGridManagerInterface
{
	GENERATED_BODY()
	/* ------------------------------------------ MEMBERS -------------------------------------------*/
protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "CC|Runtime")
	FString RawName;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "CC|Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;


public:
	FOnTileEffectTriggered OnDeathEffect;
	FOnTileEffectTriggered OnSpawnEffect;
	FOnTileEffectTriggered OnKillEffect;

	UPROPERTY()
	FGuid UnitGuid;

	UPROPERTY(Replicated)
	FIntPoint CurrentCoordinates;

	UPROPERTY()
	FDataTableRowHandle CardDataRowHandle;

	UPROPERTY()
	ETeam Team;
	
	UFUNCTION()
	void DestroyPiece();

	UFUNCTION(NetMulticast, Unreliable)
	void MLC_DestroyPiece();
	
	//Init Unit (GUID, Team, TargetMap)
	UFUNCTION()
	virtual void InitUnit(FIntPoint StartCoordinates, ETeam InTeam, const TArray<FUnitMovementData>& InPattern, const FGuid& NewGuid, FDataTableRowHandle
				  InCardDataRowHandle);

	UFUNCTION(NetMulticast, Reliable)
	virtual void MLC_InitUnit(FIntPoint StartCoordinates, ETeam InTeam, const TArray<FUnitMovementData>& InPattern, const FGuid& NewGuid, FDataTableRowHandle
				  InCardDataRowHandle);
	
	/* ----------------------------------------- FUNCTIONS -------------------------------------------*/
public:
	ACCPieceBase( const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/* ------------------------------------------ GETTERS/SETTERS -------------------------------------------*/
public:
	ETeam GetTeam() {return Team;};

	UFUNCTION()
	void SetTeam(ETeam InTeam) {Team = InTeam;}
};
