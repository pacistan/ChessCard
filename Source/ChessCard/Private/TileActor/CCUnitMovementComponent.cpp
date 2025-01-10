// Fill out your copyright notice in the Description page of Project Settings.


#include "TileActor/CCUnitMovementComponent.h"

#include "GameModes/CCGameState.h"
#include "Kismet/KismetMathLibrary.h"
#include "TileActor/CCTileUnit.h"


// Sets default values for this component's properties
UCCUnitMovementComponent::UCCUnitMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UCCUnitMovementComponent::BeginPlay()
{
	Super::BeginPlay();
	OwnerUnit = Cast<ACCTileUnit>(GetOwner());
}


// Called every frame
void UCCUnitMovementComponent::TickComponent(float DeltaTime, ELevelTick TickType,
                                             FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if(InterpolateValue <= 1 && IsMoving)
	{
		MovementTick(DeltaTime);
	}
}

void UCCUnitMovementComponent::StartMovement(FIntPoint InStartCoordinates, const FPlayerActionData& InAction, FTileActorMovementDelegate InTileActorMovementDelegate, ACCPlayerState* InPlayerState)
{
	InterpolateValue = 0;
	ActionData = InAction;
	UnitMovementData.Empty();
	UnitMovementData.Reserve(InAction.MovementData.Num() + 1);
	UnitRotationData.Reserve(InAction.MovementData.Num() + 1);
	
	ACCGridManager* GridManager	= GetWorld()->GetGameState<ACCGameState>()->GetGridManager();
	PlayerState = InPlayerState;
	IsMoving = true;

	UnitMovementData.Add(GetOwner()->GetActorLocation());
	UnitRotationData.Add(GetOwner()->GetActorRotation());
	
	FIntPoint ProgressCoordinates = InStartCoordinates;
	for(int i = 0; i < InAction.MovementData.Num(); i++)
	{
		ProgressCoordinates += InAction.MovementData[i].Direction;
		UnitMovementData.Add(GridManager->CoordinatesToPosition(ProgressCoordinates) + 20 * FVector::UpVector);
		FVector DirectionalVector = FVector(InAction.MovementData[i].Direction.X, InAction.MovementData[i].Direction.Y, 0);
		UnitRotationData.Add(UKismetMathLibrary::MakeRotFromYZ(DirectionalVector, FVector::UpVector));
		if(InAction.MovementData[i].MovementType == EMovementType::Stoppable)
		{
			break;
		}
	}
	TileActorMovementDelegate = InTileActorMovementDelegate;
}

void UCCUnitMovementComponent::MovementTick(float DeltaTime)
{
	InterpolateValue += DeltaTime / MovementDuration;
	float InterpolateTileValue = 1 / (UnitMovementData.Num() + .00001f);
	int MvtDataIndex = FMath::FloorToInt(InterpolateValue / InterpolateTileValue);
	float EvaluatedValue = FMath::Modulo(InterpolateValue, InterpolateTileValue) / InterpolateTileValue;
	
	
	
	FVector Position = FMath::Lerp(
			UnitMovementData[FMath::Min(MvtDataIndex, UnitMovementData.Num() - 1)],
			UnitMovementData[FMath::Min(MvtDataIndex + 1, UnitMovementData.Num() - 1)],
			MovementAnimCurve->FloatCurve.Eval(EvaluatedValue));

	FRotator Rotation = FQuat::Slerp(
			UnitRotationData[FMath::Min(MvtDataIndex, UnitMovementData.Num() - 1)].Quaternion(),
			UnitRotationData[FMath::Min(MvtDataIndex + 1, UnitMovementData.Num() - 1)].Quaternion(),
			MovementAnimCurve->FloatCurve.Eval(EvaluatedValue)).Rotator();
	
	GetOwner()->SetActorLocation(Position);
	GetOwner()->SetActorRotation(Rotation);
	
	if(InterpolateValue >= 1)
	{
		GetOwner()->SetActorLocation(UnitMovementData.Last());
		GetOwner()->SetActorRotation(UnitRotationData.Last());
		TileActorMovementDelegate.ExecuteIfBound(PlayerState, ActionData, OwnerUnit);
		IsMoving = false;
	}
}

