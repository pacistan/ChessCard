// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Player/CCPlayerPawn.h"
#include "CCUnitMovementComponent.generated.h"

class ACCTileUnit;
struct FPatternMapEndPoint;

DECLARE_DYNAMIC_DELEGATE_ThreeParams(FTileActorMovementDelegate, ACCPlayerState*, PlayerState, FPlayerActionData, Action, ACCPieceBase*, Piece);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CHESSCARD_API UCCUnitMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UCCUnitMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	UPROPERTY()
	TObjectPtr<ACCTileUnit> OwnerUnit;
	
	UPROPERTY(EditAnywhere)
	float MovementDuration;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UCurveFloat> MovementAnimCurve;
	
	UPROPERTY()
	float InterpolateValue;
	
	UPROPERTY()
	FTileActorMovementDelegate TileActorMovementDelegate;

	UPROPERTY()
	TArray<FVector> UnitMovementData;

	UPROPERTY()
	TArray<FRotator> UnitRotationData;

	UPROPERTY()
	FPlayerActionData ActionData;
	
	UPROPERTY()
	TObjectPtr<ACCPlayerState> PlayerState;

	UPROPERTY()
	bool IsMoving;
	
public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION()
	void StartMovement(FIntPoint InStartCoordinates, const FPlayerActionData& InAction, FTileActorMovementDelegate
	                   InTileActorMovementDelegate, ACCPlayerState* InPlayerState);

	UFUNCTION()
	void MovementTick(float DeltaTime);
};
