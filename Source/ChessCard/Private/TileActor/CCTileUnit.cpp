#include "TileActor/CCTileUnit.h"
#include "TileActor/PatternMapEndPoint.h"
#include "Card/CCCard.h"
#include "GameModes/CCGameState.h"
#include "Grid/CCGridManager.h"
#include "Grid/CCTile.h"
#include "Hand/CCHandComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Macro/CCLogMacro.h"
#include "Player/CCPlayerController.h"
#include "Player/CCPlayerPawn.h"
#include "GameModes/CCGameState.h"
#include "Player/CCPlayerState.h"
#include "TileActor/CCUnitMovementComponent.h"

ACCTileUnit::ACCTileUnit(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MovementComponent = CreateDefaultSubobject<UCCUnitMovementComponent>(TEXT("Movement"));
}

void ACCTileUnit::SetHighlight(bool bToHighlight, FOnClickUnitDelegate InOnClickDelegate,
	FOnHoverUnitDelegate InOnHoverUnitDelegate)
{
	IsHighlighted = bToHighlight;
	UMaterialInterface* NewMaterial = IsHighlighted ? HighlightMat : BaseMaterial;
	MeshComponent->SetMaterial(0, NewMaterial);
	//FLinearColor Color = IsHighlighted ? HighlightColor : BaseColor;
	//MaterialInstance->SetVectorParameterValue("Color", Color);
}

void ACCTileUnit::HighlightDestinationTiles(ACCPlayerPawn* Pawn)
{
	auto GridManager = GetGridManager(GetWorld());
	check(GridManager);
	check(Pawn);
	
	GridManager->UnhighlightTiles();

	for(const auto& MovementData : PatternMap)
	{
		if(MovementData.Value.MovementType == EMovementType::Stoppable)
		{
			ACCTile* Tile = GridManager->GetTile(MovementData.Key + CurrentCoordinates);
			//TODO: Add true if TileType is the one of Owning PlayerPawn
			if(IsValid(Tile) && (Tile->GetTileType() == ETileType::Normal ||
								 Tile->GetTileType() == ETileType::BonusTile ||
								 Tile->GetTileType() == ETileType::ScoreTile))
			{
			    ACCCard* Card = Pawn->GetHandComponent()->Cards[Pawn->GetCurrentSelectedCardIndex()];
				check(Card);
				if(IsValid(Card))
				{
					FOnClickTileDelegate OnClickDelegate;
					OnClickDelegate.BindDynamic(Card, &ACCCard::ACCCard::MoveUnit);
					if(OnClickDelegate.IsBound())
					{
						Tile->SetHighlight(true, OnClickDelegate);
						//Tile->OnClickEvent = OnClickDelegate;
					}					
				}
				
			}
		}
		else if(MovementData.Value.MovementType == EMovementType::ApplyEffect)
		{
			//TODO: Visual representation of Effect
		}
	}
}

void ACCTileUnit::OnDestinationTileClicked(ACCTile* Tile)
{
	check(Tile);
	ACCGameState* GameState = GetWorld()->GetGameState<ACCGameState>();
	check(GameState);
	ACCPlayerState* PlayerState = GameState->GetPlayerStateOfTeam(Team);
	check(PlayerState);
	ACCPlayerPawn* Player = PlayerState->GetPawn<ACCPlayerPawn>();
	check(Player);
	ACCCard* Card = Player->GetHandComponent()->Cards[Player->GetCurrentSelectedCardIndex()];
	check(Card);
	Card->MoveUnit(Tile);
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
	BaseMaterial = MeshComponent->GetMaterial(0);
	//MaterialInstance = UMaterialInstanceDynamic::Create(MeshComponent->GetMaterial(0), this);
	//FName ColorParameterName = FName("Color");
	//MaterialInstance->GetVectorParameterValue(ColorParameterName, BaseColor);

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
		Select(Player);
	}
}
