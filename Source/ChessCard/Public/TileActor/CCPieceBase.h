// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Card/FCardData.h"
#include "GameFramework/Actor.h"
#include "CCPieceBase.generated.h"

class ACCPlayerState;
class ACCPlayerController;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnTileEffectTriggered, ACCPieceBase* , Owner);

/**
 *  Base class for all the Actor that will be placed on the board
 */
UCLASS()
class CHESSCARD_API ACCPieceBase : public AActor
{
	GENERATED_BODY()
	/* ------------------------------------------ MEMBERS -------------------------------------------*/
protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "CC|Runtime")
	FString RawName;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "CC|Components")
	TObjectPtr<UStaticMeshComponent> MeshComponent;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "CC|Runtime")
	FMovementPattern MovementPattern = FMovementPattern();
	
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "CC|Runtime")
	int Level = 1;
	
	FIntVector2 Position;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "CC|Runtime")
	ACCPlayerState* OwnerPlayerState;

public:
	FOnTileEffectTriggered OnDeathEffect;
	FOnTileEffectTriggered OnSpawnEffect;
	FOnTileEffectTriggered OnKillEffect;
	
	/* ----------------------------------------- FUNCTIONS -------------------------------------------*/
public:
	ACCPieceBase( const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void Initialize(ACCPlayerState* PlayerState, FString RawName, FIntVector2 Position, FMovementPattern MovementPattern, int Level = 1);  
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
  
};
