#pragma once

#include "CoreMinimal.h"
#include "Card/FCardData.h"
#include "GameFramework/Actor.h"
#include "Card/FUnitMovementData.h"
#include "Interfaces/CCGridManagerInterface.h"
#include "CCPieceBase.generated.h"

enum class ETeam : uint8;
class ACCPlayerState;
class ACCPlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTileEffectTriggered, ACCPieceBase* , Owner);

USTRUCT()
struct FInitilizationProperties
{
	GENERATED_BODY()

public:
	UPROPERTY()
	FIntPoint Coordinates;

	UPROPERTY()
	ETeam Team;

	UPROPERTY()
	FGuid InstanceID;

	UPROPERTY()
	FDataTableRowHandle CardDataRowHandle;

	FInitilizationProperties(){}
	
	FInitilizationProperties(FIntPoint InCoordinates, ETeam InTeam, FGuid InInstanceID, FDataTableRowHandle InCardDataRowHandle):
	Coordinates(InCoordinates), Team(InTeam), InstanceID(InInstanceID), CardDataRowHandle(InCardDataRowHandle){}
};

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

	UPROPERTY(ReplicatedUsing = OnRep_InitProperties)
	FInitilizationProperties InitilizationProperties;

	UPROPERTY()
	bool IsInitialized;
	
	UPROPERTY()
	FGuid UnitGuid;

	UPROPERTY(Replicated)
	FIntPoint CurrentCoordinates;

	UPROPERTY(Replicated)
	FIntPoint PreviousCoordinates;
	
	UPROPERTY()
	FDataTableRowHandle CardDataRowHandle;

	UPROPERTY()
	ETeam Team;

	UPROPERTY()
	bool IsSelected;
	
	/* ----------------------------------------- FUNCTIONS -------------------------------------------*/
public:
	ACCPieceBase( const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION()
	void DestroyPiece();

	UFUNCTION(NetMulticast, Unreliable)
	void MLC_DestroyPiece();
	
	//Init Unit (GUID, Team, TargetMap)
	UFUNCTION()
	virtual void InitUnit(const FInitilizationProperties& InInitializationProperties);

	UFUNCTION()
	virtual void InternalInit();

	UFUNCTION()
	void OnRep_InitProperties();

	UFUNCTION(BlueprintImplementableEvent)
	void ConstructTile(FCardData Card);
	
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/* ------------------------------------------ GETTERS/SETTERS -------------------------------------------*/
public:
	ETeam GetTeam() {return Team;};

	UFUNCTION()
	void SetTeam(ETeam InTeam) {Team = InTeam;}

	UFUNCTION()
	virtual void UnSelect();

};
