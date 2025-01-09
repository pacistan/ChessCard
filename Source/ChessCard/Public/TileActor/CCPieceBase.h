#pragma once

#include "CoreMinimal.h"
#include "Card/FCardData.h"
#include "GameFramework/Actor.h"
#include "Card/FUnitMovementData.h"
#include "Interfaces/CCGridManagerInterface.h"
#include "CCPieceBase.generated.h"

class ACCSplineMeshActor;
class UWidgetComponent;
enum class ETeam : uint8;
class ACCPlayerState;
class ACCPlayerController;
class USplineMeshComponent;
class USplineComponent;
class ACCSplineMeshActor;

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

	bool IsPreview = false;
	
	FInitilizationProperties(){}
	
	FInitilizationProperties(FIntPoint InCoordinates, ETeam InTeam, FGuid InInstanceID, FDataTableRowHandle InCardDataRowHandle, bool InIsPreview = false):
	Coordinates(InCoordinates), Team(InTeam), InstanceID(InInstanceID), CardDataRowHandle(InCardDataRowHandle), IsPreview(InIsPreview){}
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

	UPROPERTY(VisibleInstanceOnly, BlueprintReadWrite, Category = "CC|Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CC|Components")
	TObjectPtr<UWidgetComponent> EffectContainerWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CC|Components")
	TObjectPtr<UWidgetComponent> DeathIndicatorWidget;

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

	UPROPERTY()
	TArray<TObjectPtr<USplineMeshComponent>> SplineMeshComponents;

	UPROPERTY(EditAnywhere, Category="", meta=(AllowPrivateAccess))
	TObjectPtr<USplineComponent> SplineComponent;

	UPROPERTY(EditDefaultsOnly, Category = "CC|SPLINE")
	TSubclassOf<ACCSplineMeshActor> SplineClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "CC|SPLINE")
	UStaticMesh* SplineMesh;

	UPROPERTY(EditDefaultsOnly, Category = "CC|SPLINE")
	UMaterialInterface* SplineMaterial;
	
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
    void CreateSpline(TArray<FVector> Positions, TArray<AActor*>& ActionVisuals);
	
	UFUNCTION()
	void SetSplinePoints();

	UFUNCTION()
	void ClearSpline();
	
	UFUNCTION()
	void OnRep_InitProperties();

	UFUNCTION(Client, Unreliable)
	void RPC_SetVisible();
	
	UFUNCTION(BlueprintImplementableEvent)
	void BPE_ConstructTile(FDataTableRowHandle Card, UMaterialInterface* SocleMaterial, bool IsPreview);
	
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	/* ------------------------------------------ GETTERS/SETTERS -------------------------------------------*/
public:
	ETeam GetTeam() {return Team;};

	UFUNCTION()
	void SetTeam(ETeam InTeam) {Team = InTeam;}

	UFUNCTION()
	virtual void UnSelect();

	UFUNCTION(BlueprintImplementableEvent)
	void BPE_OnClickUnit();

	UFUNCTION(BlueprintImplementableEvent)
	void BPE_OnUnitDestroy(bool ActivateEffects);
};
