#include "Card/CCCard.h"

#include "Card/CCCardMovementComponent.h"
#include "Card/FCardData.h"
#include "GameModes/CCGameState.h"
#include "Grid/CCGridManager.h"
#include "Grid/CCTile.h"
#include "Hand/CCHandComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Macro/CCLogMacro.h"
#include "TileActor/CCPieceBase.h"
#include "TileActor/CCTileUnit.h"


ACCCard::ACCCard()
{
	PrimaryActorTick.bCanEverTick = true;
	CardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Card Sleeve"));
	RootComponent = CardMesh;
	if(!IsValid(CardMesh->GetStaticMesh()))
	{
		ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(TEXT("/Engine/BasicShapes/Cube.Cube"));
		CardMesh->SetStaticMesh(MeshRef.Object);
	}
	CardMovement = CreateDefaultSubobject<UCCCardMovementComponent>(TEXT("Card Movement"));
}

void ACCCard::UpdateMaterials()
{
	for(int i = 0; i < MaterialMap[CurrentCardState].Materials.Num(); i++)
	{
		CardMesh->SetMaterial(i, MaterialMap[CurrentCardState].Materials[i]);
	}
}

void ACCCard::OnSelectCardEffects(bool bIsSelected, ACCPlayerPawn* Pawn)
{
	FCardData* RowData = CardRowHandle.GetRow<FCardData>( GetNameSafe(this));
	
	switch(RowData->CardType)
	{
		case ECardType::Unit:
		{
			ETileType TargetTileType = ETileType::Player1;
			switch(Pawn->GetPlayerIndex())
			{
				case 1:
					TargetTileType = ETileType::Player1;
					break;
				case 2:
					TargetTileType = ETileType::Player2;
					break;
				case 3:
					TargetTileType = ETileType::Player3;
					break;
				case 4:
					TargetTileType = ETileType::Player4;
					break;
				default:
					//DEBUG_ERROR("Pawn has an invalid Player Index : %i", Pawn->GetPlayerIndex());
					break;
			}
				
			/*if(bIsSelected)
			{
				GetGridManager(	GetWorld())->UnhighlightTiles();
			}*/
				
			FTileTypeDelegate TileTypeDelegate;
			auto Lambda = [this, &bIsSelected](ACCTile* Tile)
			{
				if(!Tile->ContainPiece())
				{
					FOnClickTileDelegate OnClickDelegate;
					OnClickDelegate.BindDynamic(this, &ACCCard::SpawnUnit);
					Tile->SetHighlight(bIsSelected, OnClickDelegate);
				}
			};
			TileTypeDelegate.BindLambda(Lambda);
			GetGridManager(GetWorld())->ApplyLambdaToTileType(TargetTileType, TileTypeDelegate);
			break;
		}
		case ECardType::Movement:
		{
			bool ToHighlight = bIsSelected;
			FTileTypeDelegate TileTypeDelegate;
			auto Lambda = [this, &ToHighlight](ACCTile* Tile)
			{
				for(auto& Piece : Tile->GetPieces())
				{
					auto Unit = Cast<ACCTileUnit>(Piece);
					if(IsValid(Unit))
					{
						Unit->SetHighlight(ToHighlight);
					}
				}
			};
			TileTypeDelegate.BindLambda(Lambda);
			GetGridManager(GetWorld())->ApplyLambdaToTileType(ETileType::Unit, TileTypeDelegate);
			break;
		}
		case ECardType::SpecificUnit:
			DEBUG_WARNING("Not Implemented Specific Unit Card Type");
			break;
		case ECardType::Custom:
			DEBUG_WARNING("Not Implemented Custom Card Type");
			break;
	}
}

void ACCCard::BeginPlay()
{
	Super::BeginPlay();
}

void ACCCard::Play(ACCPlayerPawn* Pawn)
{
	CurrentCardState = ECardState::Played;
	UpdateMaterials();
	if(!CardMovement->IsInterruptable)
	{
		return;
	}

	CardMovement->StartMovement(CardIndex, Pawn->GetHandComponent()->GetCardNum());
}

void ACCCard::SpawnUnit(ACCTile* Tile)
{
	const FRotator UnitRotation;
	const FVector UnitPosition = Tile->GetActorLocation() + FVector::UpVector * 20;
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.bNoFail = true;
	auto Unit =  GetWorld()->SpawnActor<ACCTileUnit>(PieceClass, UnitPosition, UnitRotation, SpawnParams);
	Tile->AddPieceLocal(Unit);
	Unit->CurrentCoordinates = FIntPoint(Tile->GetRowNum(), Tile->GetColumnNum());
	if (FCardData* CardData = CardRowHandle.GetRow<FCardData>( GetNameSafe(this))){
		Unit->Pattern = CardData->Pattern;
	}
	Unit->SetTargetMap();
	auto GridManager = GetGridManager(GetWorld());
	check(GridManager);
	GridManager->UnhighlightTiles();
	//TODO : Add Summon Action to Queue of Actions For Resolve and historic.
}

void ACCCard::MoveUnit(ACCTile* Unit)
{
	
}

void ACCCard::StartHover(ACCPlayerPawn* Pawn)
{
	if(CurrentCardState == ECardState::Played || CurrentCardState == ECardState::Selected || !CardMovement->IsInterruptable)
		return;
	CurrentCardState = ECardState::Hovered;
	
	CardMovement->StartMovement(CardIndex, Pawn->GetHandComponent()->GetCardNum());
	UpdateMaterials();
}

void ACCCard::StopHover(ACCPlayerPawn* Pawn)
{
	if(!CardMovement->IsInterruptable || CurrentCardState != ECardState::Hovered)
	{
		return;
	}
	
	CurrentCardState = ECardState::Inactive;
	CardMovement->StartMovement(CardIndex, Pawn->GetHandComponent()->GetCardNum());
	UpdateMaterials();
}

void ACCCard::Select(ACCPlayerPawn* Pawn)
{
	//TODO: Check if In Planning Mode
	if(!CardMovement->IsInterruptable)
	{
		return;
	}
	
	bool bToSelected = true;
	if(CurrentCardState == ECardState::Played)
	{
		bToSelected = false;
		CurrentCardState = ECardState::Hovered;
		Pawn->GetPlayedCardIndex().Remove(CardIndex);

		CardMovement->StartMovement(CardIndex, Pawn->GetHandComponent()->GetCardNum());
		UpdateMaterials();
		//TODO: Unplay movements AND/OR Spawns
		return;
	}
	if(CurrentCardState == ECardState::Selected)
	{
		CurrentCardState = ECardState::Hovered;
		Pawn->SetCurrentSelectedCardIndex(-1);
		bToSelected = false;
	}
	else
	{
		CurrentCardState = ECardState::Selected;
		Pawn->SetCurrentSelectedCardIndex(CardIndex);
	}
	
	CardMovement->StartMovement(CardIndex, Pawn->GetHandComponent()->GetCardNum());
	UpdateMaterials();
	OnSelectCardEffects(bToSelected, Pawn);
}

void ACCCard::UnSelect(ACCPlayerPawn* Pawn)
{
	
	//TODO: Check if In Planning Mode
	if(!CardMovement->IsInterruptable)
	{
		return;
	}
	CurrentCardState = CurrentCardState ==  ECardState::Played ? ECardState::Played : ECardState::Inactive;
	Pawn->SetCurrentSelectedCardIndex(-1);
	CardMovement->StartMovement(CardIndex, Pawn->GetHandComponent()->GetCardNum());
	UpdateMaterials();
	OnSelectCardEffects(false, Pawn);
}

