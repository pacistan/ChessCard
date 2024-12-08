#include "TileActor/CCTileUnit.h"

#include "Card/CCCard.h"
#include "Grid/CCGridManager.h"
#include "Grid/CCTile.h"
#include "Hand/CCHandComponent.h"
#include "Player/CCPlayerPawn.h"
#include "TileActor/CCUnitMovementComponent.h"

ACCTileUnit::ACCTileUnit(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MovementComponent = CreateDefaultSubobject<UCCUnitMovementComponent>(TEXT("Movement"));
}

void ACCTileUnit::SetHighlight(bool bToHighlight, FOnClickUnitDelegate InOnClickDelegate,
	FOnHoverUnitDelegate InOnHoverUnitDelegate)
{
	IsHighlighted = bToHighlight;
	FLinearColor Color = IsHighlighted ? HighlightColor : BaseColor;
	MaterialInstance->SetVectorParameterValue("Color", Color);
}

void ACCTileUnit::HighlightDestinationTiles()
{
	auto GridManager = GetGridManager(GetWorld());
	check(GridManager);
	
	GridManager->UnhighlightTiles();

	for(int i = 0; i < Pattern.Num(); i++)
	{
		for(const auto& MovementData : PatternMap)
		{
			if(Pattern[i].MovementType == EMovementType::Stoppable)
			{
				ACCTile* Tile = GridManager->GetTile(MovementData.Value[i] + CurrentCoordinates);
				if(IsValid(Tile) && (Tile->GetTileType() == ETileType::Normal || Tile->GetTileType() == ETileType::BonusTile || Tile->GetTileType() == ETileType::ScoreTile))
				{
					auto Pawn = Cast<ACCPlayerPawn>(GetWorld()->GetFirstPlayerController()->GetPawn());
					auto Card = Pawn->GetHandComponent()->Cards[Pawn->GetCurrentSelectedCardIndex()];
					FOnClickTileDelegate OnClickDelegate;
					OnClickDelegate.BindDynamic(Card, &ACCCard::MoveUnit);
					Tile->SetHighlight(true, OnClickDelegate);
				}
			}
			else if(Pattern[i].MovementType == EMovementType::ApplyEffect)
			{
				//TODO: Visual representation of Effect
			}
		}
	}
}

void ACCTileUnit::Select(ACCPlayerPawn* Player)
{
	IsSelected = true;
	Player->SetSelectedUnit(this);
}

void ACCTileUnit::UnSelect()
{
	if(IsSelected)
	{
		GetGridManager(GetWorld())->UnhighlightTiles();
	}

	IsSelected = false;
	SetHighlight(false);
}

void ACCTileUnit::SetTargetMap()
{
	auto GridManager = GetGridManager(GetWorld());
	check(GridManager);
	GridManager->GetTargetTiles(Pattern, PatternMap);
}

void ACCTileUnit::BeginPlay()
{
	Super::BeginPlay();
	MaterialInstance = UMaterialInstanceDynamic::Create(MeshComponent->GetMaterial(0), this);
	FName ColorParameterName = FName("Color");
	MaterialInstance->GetVectorParameterValue(ColorParameterName, BaseColor);

}

void ACCTileUnit::StartHover(ACCPlayerPawn* Player)
{
	if(IsHighlighted)
	{
		HighlightDestinationTiles();
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
		Select(Player);
	}
}
