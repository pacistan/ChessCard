#include "TileActor/CCTileUnit.h"
#include "TileActor/PatternMapEndPoint.h"
#include "Card/CCCard.h"
#include "GameModes/CCGameState.h"
#include "Grid/CCGridManager.h"
#include "Grid/CCTile.h"
#include "Hand/CCHandComponent.h"
#include "Player/CCPlayerPawn.h"
#include "Player/CCPlayerState.h"
#include "TileActor/CCUnitMovementComponent.h"

ACCTileUnit::ACCTileUnit(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MovementComponent = CreateDefaultSubobject<UCCUnitMovementComponent>(TEXT("Movement"));
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
	GridManager->GetTargetTiles(Pattern, PatternMap);
}

void ACCTileUnit::HighlightDestinationTiles(ACCPlayerPawn* Pawn)
{
	auto GridManager = GetGridManager(GetWorld());
	ACCCard* Card = Pawn->GetHandComponent()->Cards[Pawn->GetCurrentSelectedCardIndex()];
	check(Card);
	check(GridManager);
	check(Pawn);
	   
	GridManager->UnhighlightTiles();

	for(const auto& MovementData : PatternMap)
	{
		FIntPoint Coordinates = CurrentCoordinates;
		for(auto& Coordinate : MovementData.Value)
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
					break;
				case EMovementType::Stoppable:
					HighlightMode = EHighlightMode::Normal;
					OnClickTileDelegate.BindDynamic(Card, &ACCCard::ACCCard::MoveUnit);
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