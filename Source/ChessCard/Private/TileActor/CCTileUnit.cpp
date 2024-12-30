#include "TileActor/CCTileUnit.h"
#include "TileActor/PatternMapEndPoint.h"
#include "Card/CCCard.h"
#include "GameModes/CCGameState.h"
#include "Grid/CCGridManager.h"
#include "Grid/CCTile.h"
#include "Hand/CCHandComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Player/CCPlayerPawn.h"
#include "Player/CCPlayerState.h"
#include "TileActor/CCUnitMovementComponent.h"

ACCTileUnit::ACCTileUnit(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MovementComponent = CreateDefaultSubobject<UCCUnitMovementComponent>(TEXT("Movement"));
}

FIntPoint ACCTileUnit::GetTravelRelativeCoordinates(TArray<FPatternMapEndPoint>& PatternMovement)
{
	FIntPoint EndPoint = FIntPoint();
	for(auto Movement : PatternMovement)
	{
		EndPoint += Movement.Direction;
	}
	return EndPoint;
}

void ACCTileUnit::BeginPlay()
{
	Super::BeginPlay();
	BaseMaterial = MeshComponent->GetMaterial(0);
}

void ACCTileUnit::SetTargetMap()
{
	auto GridManager = GetGridManager(GetWorld());
	check(GridManager);
	GridManager->GetTargetTiles(Pattern, PatternList);
}

void ACCTileUnit::HighlightDestinationTiles(ACCPlayerPawn* Pawn)
{
	auto GridManager = GetGridManager(GetWorld());
	LinkedCard = Pawn->GetHandComponent()->Cards[Pawn->GetCurrentSelectedCardIndex()];
	check(LinkedCard);
	check(GridManager);
	check(Pawn);
	   
	GridManager->UnhighlightTiles();

	for(auto& MvtData : PatternList)
	{
		FIntPoint Coordinates = CurrentCoordinates;
		FIntPoint Destination = Coordinates + GetTravelRelativeCoordinates(MvtData);
		ACCTile* DestinationTile = GridManager->GetTile(Destination);
		if(!IsValid(DestinationTile) ||!DestinationTile->IsAccessibleForTeam(Pawn->GetPlayerState<ACCPlayerState>()->GetTeam()))
		{
			continue;
		}
		for(auto& Coordinate : MvtData)
		{
			Coordinates += Coordinate.Direction;
			ACCTile* Tile = GridManager->GetTile(Coordinates);
			if(IsValid(Tile) && Tile->GetTileType() != ETileType::Disabled)
			{
				EHighlightMode HighlightMode;
				FOnClickTileDelegate OnClickTileDelegate;
				switch(Coordinate.MovementType)
				{
				case EMovementType::Normal:
					HighlightMode = EHighlightMode::Path;
					Tile->SetHighlight(true, OnClickTileDelegate, HighlightMode);
					break;
				case EMovementType::Stoppable:
					HighlightMode = EHighlightMode::Normal;
					OnClickTileDelegate.BindDynamic(this, &ACCTileUnit::OnDestinationTileClicked);
					Tile->SetHighlight(true, OnClickTileDelegate, HighlightMode);
					break;
				case EMovementType::ApplyEffect:
					HighlightMode = EHighlightMode::Effect;
					Tile->SetHighlight(true, OnClickTileDelegate, HighlightMode);
					break;
				default:
					HighlightMode = EHighlightMode::Normal;
				}
			}
		}
	}
}

void ACCTileUnit::OnDestinationTileClicked(ACCTile* Tile)
{
	FIntPoint EndPoint = FIntPoint(Tile->GetRowNum(), Tile->GetColumnNum());
	FIntPoint Travel = EndPoint - CurrentCoordinates;
	TArray<FPatternMapEndPoint> OutPatternMovement;
	for(auto PatternMovement : PatternList)
	{
		if(Travel == GetTravelRelativeCoordinates(PatternMovement))
		{
			OutPatternMovement = PatternMovement;
			break;
		}
	}
	if(IsValid(MovementPointActorClass) && IsValid(DestinationPointActorClass))
	{
		FIntPoint ProgressPoint = CurrentCoordinates;
		FActorSpawnParameters SpawnParameters;
		for(int i = 0; i < OutPatternMovement.Num(); i++)
		{
			ProgressPoint += OutPatternMovement[i].Direction;
			FVector PointPosition =  GetGridManager(GetWorld())->CoordinatesToPosition(ProgressPoint);
			TSubclassOf<AActor> Subclass = i == OutPatternMovement.Num() - 1 ? DestinationPointActorClass : MovementPointActorClass;
			FVector Direction = FVector(OutPatternMovement[i].Direction.X, OutPatternMovement[i].Direction.Y, 0);
			FRotator Rotator = UKismetMathLibrary::MakeRotFromXZ(Direction, FVector::UpVector);
			GetWorld()->SpawnActor<AActor>(Subclass, PointPosition, Rotator, SpawnParameters); 
		}
	}
	
	LinkedCard->MoveUnit(Tile, OutPatternMovement);
}

void ACCTileUnit::SetHighlight(bool bToHighlight, FOnClickUnitDelegate InOnClickDelegate,
	FOnHoverUnitDelegate InOnHoverUnitDelegate)
{
	IsHighlighted = bToHighlight;
	UMaterialInterface* NewMaterial = IsHighlighted ? HighlightMat : BaseMaterial;
	MeshComponent->SetMaterial(0, NewMaterial);
}

void ACCTileUnit::StartHover(ACCPlayerPawn* Player)
{
	if(IsHighlighted)
	{
		HighlightDestinationTiles(Player);
	}
}

void ACCTileUnit::StopHover(ACCPlayerPawn* Player)
{
	if(!IsSelected)
	{
		GetGridManager(GetWorld())->UnhighlightTiles();
	}
}

void ACCTileUnit::Click(ACCPlayerPawn* Player)
{
	if(IsHighlighted)
	{
		IsSelected = true;
		MeshComponent->SetMaterial(0, SelectedMaterial);
		Player->SetSelectedUnit(this);
	}
}

void ACCTileUnit::UnSelect()
{
	if(IsSelected)
	{
		GetGridManager(GetWorld())->UnhighlightTiles();
		MeshComponent->SetMaterial(0, BaseMaterial);
		IsSelected = false;
	}
}