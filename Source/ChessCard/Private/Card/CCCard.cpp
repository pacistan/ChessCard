#include "Card/CCCard.h"

#include "Card/CCCardMovementComponent.h"
#include "Card/FCardData.h"
#include "GameModes/CCEffectManagerComponent.h"
#include "GameModes/CCGameState.h"
#include "Grid/CCGridManager.h"
#include "Grid/CCTile.h"
#include "Hand/CCHandComponent.h"
#include "Macro/CCLogMacro.h"
#include "Player/CCPlayerPawn.h"
#include "Player/CCPlayerState.h"
#include "TileActor/CCPieceBase.h"
#include "TileActor/CCTileUnit.h"


ACCCard::ACCCard()
{
	PrimaryActorTick.bCanEverTick = true;
	CCRootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	RootComponent = CCRootComponent;
	//CardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Card Sleeve"));
	//CardMesh->SetupAttachment(CCRootComponent);
	//if(!IsValid(CardMesh->GetStaticMesh()))
	//{
	//	ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(TEXT("/Engine/BasicShapes/Cube.Cube"));
	//	CardMesh->SetStaticMesh(MeshRef.Object);
	//}
	CardMovement = CreateDefaultSubobject<UCCCardMovementComponent>(TEXT("Card Movement"));
}

/*void ACCCard::UpdateMaterials()
{
	for(int i = 0; i < MaterialMap[CurrentCardState].Materials.Num(); i++)
	{
		CardMesh->SetMaterial(i, MaterialMap[CurrentCardState].Materials[i]);
	}
}*/

void ACCCard::OnSelectCardEffects(bool bIsSelected, ACCPlayerPawn* Pawn)
{
	FCardData* RowData = CardRowHandle.GetRow<FCardData>( GetNameSafe(this));


	ACCPlayerState* PlayerState = Pawn->GetPlayerState<ACCPlayerState>();
	
	switch(RowData->CardType)
	{
		case ECardType::Unit:
		{
			{
				ETileType TargetTileType = ETileType::Player1;
				switch(PlayerState->GetTeam())
				{
					case ETeam::One:
						TargetTileType = ETileType::Player1;
						break;
					case ETeam::Two:
						TargetTileType = ETileType::Player2;
						break;
					case ETeam::Three:
						TargetTileType = ETileType::Player3;
						break;
					case ETeam::Four:
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
						OnClickDelegate.BindDynamic(this, &ACCCard::SpawnLocalUnit);
						Tile->SetHighlight(bIsSelected, OnClickDelegate);
					}
				};
				TileTypeDelegate.BindLambda(Lambda);
				GetGridManager(GetWorld())->ApplyLambdaToTileType(TargetTileType, TileTypeDelegate);
				
			}
			break;
		}
		case ECardType::Movement:
		{
			{
				FTileTypeDelegate TileTypeDelegate;
				auto Lambda = [this, &bIsSelected](ACCTile* Tile)
				{
					for(auto& Piece : Tile->GetPieces())
					{
						auto Unit = Cast<ACCTileUnit>(Piece);
						if(IsValid(Unit) && Unit->CardDataRowHandle.GetRow<FCardData>("")->CardType == ECardType::Unit)
						{
							Unit->SetHighlight(bIsSelected, OwningPawn->GetPlayerState<ACCPlayerState>()->GetTeam());
						}
					}
				};
				TileTypeDelegate.BindLambda(Lambda);
				GetGridManager(GetWorld())->ApplyLambdaToTileType(ETileType::Unit, TileTypeDelegate);
			}
			break;
		}
		case ECardType::SpecificUnit:
			DEBUG_WARNING("Not Implemented Specific Unit Card Type");
			break;
	case ECardType::Custom:
			if(RowData->EffectType == EEffectType::Embrasement)
			{
				FTileTypeDelegate TileTypeDelegate;
				auto Lambda = [this, &bIsSelected](ACCTile* Tile)
				{
					for(auto& Piece : Tile->GetPieces())
					{
						auto Unit = Cast<ACCTileUnit>(Piece);
						if(IsValid(Unit) && Unit->CardDataRowHandle.GetRow<FCardData>("")->EffectType == EEffectType::Embrasement)
						{
							Unit->SetHighlight(bIsSelected, OwningPawn->GetPlayerState<ACCPlayerState>()->GetTeam());
						}
					}
				};
				TileTypeDelegate.BindLambda(Lambda);
				GetGridManager(GetWorld())->ApplyLambdaToTileType(ETileType::Unit, TileTypeDelegate);
			}
			break;
	}
}

void ACCCard::BeginPlay()
{
	Super::BeginPlay();
	CardUniqueID = FGuid::NewGuid();
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

void ACCCard::Initialize()
{
	ConstructCard(*CardRowHandle.GetRow<FCardData>( GetNameSafe(this)));
}

void ACCCard::SpawnLocalUnit(ACCTile* Tile)
{
	const FRotator UnitRotation;
	const FVector UnitPosition = Tile->GetActorLocation() + FVector::UpVector * 20;
	FActorSpawnParameters UnitSpawnParams;
	UnitSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	UnitSpawnParams.bNoFail = true;
	
	ACCTileUnit* Unit =  GetWorld()->SpawnActor<ACCTileUnit>(GetWorld()->GetGameState<ACCGameState>()->PieceClass, UnitPosition, UnitRotation, UnitSpawnParams);
	Unit->InitUnit(FInitilizationProperties(FIntPoint(Tile->GetRowNum(), Tile->GetColumnNum()) ,
				   OwningPawn->GetPlayerState<ACCPlayerState>()->GetTeam(), FGuid::NewGuid(), CardRowHandle));

	Unit->SetReplicates(false);
	GetGridManager(GetWorld())->UnhighlightTiles();
	TArray<AActor*> LocalVisualActors {Unit};

	FPlayerActionData PlayerActionData(CardRowHandle, Unit->CurrentCoordinates, CardUniqueID, Unit->UnitGuid);
	OwningPawn->AddPlayerAction(PlayerActionData);
	OwningPawn->AddPlayerActionClientElement(LocalVisualActors, this);
}

void ACCCard::MoveUnit(ACCTile* Tile, TArray<FPatternMapEndPoint> MovementData, TArray<AActor*>& MovementVisualActors, ACCTileUnit* Unit)
{
	GetGridManager(GetWorld())->UnhighlightTiles();

	FCardData UnitCardData = *Unit->CardDataRowHandle.GetRow<FCardData>("");
	bool IsDivineAnger = Unit->DivineAngerCounter >= UnitCardData.DivineAngerTriggerNumber;
	FPlayerActionData PlayerActionData(Unit->CardDataRowHandle, Unit->CurrentCoordinates, CardUniqueID, Unit->UnitGuid, MovementData, IsDivineAnger
		);
	
	OwningPawn->AddPlayerAction(PlayerActionData);
	OwningPawn->AddPlayerActionClientElement(MovementVisualActors, this);
}

void ACCCard::TriggerEmbrasement(ACCTile* Tile, TArray<AActor*>& MovementVisualActors, ACCTileUnit* Unit)
{
	FCardData UnitCardData = *Unit->CardDataRowHandle.GetRow<FCardData>("");
	FPlayerActionData PlayerActionData(Unit->CardDataRowHandle, Unit->CurrentCoordinates, CardUniqueID, Unit->UnitGuid, TArray<FPatternMapEndPoint>());
	
	OwningPawn->AddPlayerAction(PlayerActionData);
	OwningPawn->AddPlayerActionClientElement(MovementVisualActors, this);
	GetOwner<ACCPlayerPawn>()->PlaySelectedCard(nullptr);
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
	if(!CardMovement->IsInterruptable || CurrentCardState == ECardState::Played)
	{
		return;
	}
	
	bool bToSelected = true;
	if(CurrentCardState == ECardState::Played)
	{
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

