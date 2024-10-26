#include "Card/CCCard.h"
#include "Card/CCCardMovementComponent.h"
#include "GameModes/CCGameState.h"
#include "Grid/CCGridManager.h"
#include "Grid/CCTile.h"
#include "Hand/CCHandComponent.h"
#include "Kismet/GameplayStatics.h"

ACCCard::ACCCard()
{
	PrimaryActorTick.bCanEverTick = true;
	CardMovement = CreateDefaultSubobject<UCCCardMovementComponent>(TEXT("Card Movement"));
	CardMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Card Sleeve"));
	RootComponent = CardMesh;
	if(!IsValid(CardMesh->GetStaticMesh()))
	{
		ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(TEXT("/Engine/BasicShapes/Cube.Cube"));
		CardMesh->SetStaticMesh(MeshRef.Object);
	}
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
    ETileType TargetTileType;
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
		UE_LOG(LogTemp, Error, TEXT("Pawn has an invalid Player Index"));
		return;
	}

	bool toHighlight = bIsSelected;
	FTileTypeDelegate TileTypeDelegate;
	auto Lambda = [&toHighlight](ACCTile* Tile){Tile->SetHighlight(toHighlight);};
	TileTypeDelegate.BindLambda(Lambda);
	Cast<ACCGameState>(UGameplayStatics::GetGameState(GetWorld()))->GetGridManager()->ApplyLambdaToTileType(TargetTileType, TileTypeDelegate);
}

void ACCCard::Play(ACCPlayerPawn* Pawn)
{
	CurrentCardState = ECardState::Played;
	FOnCardMovementEnd OnCardMovementEnd;
	CardMovement->StartMovement(CardIndex, Pawn->GetHandComponent()->GetCardNum(), OnCardMovementEnd);
	UpdateMaterials();
}

void ACCCard::StartHover(ACCPlayerPawn* Pawn)
{
	if(CurrentCardState == ECardState::Played || CurrentCardState == ECardState::Selected)
		return;
	CurrentCardState = ECardState::Hovered;
	
	FOnCardMovementEnd OnCardMovementEnd;
	CardMovement->StartMovement(CardIndex, Pawn->GetHandComponent()->GetCardNum(), OnCardMovementEnd);
	UpdateMaterials();
}

void ACCCard::StopHover(ACCPlayerPawn* Pawn)
{
	if(CurrentCardState != ECardState::Hovered)
		return;
	CurrentCardState = ECardState::Inactive;
	
	FOnCardMovementEnd OnCardMovementEnd;
	CardMovement->StartMovement(CardIndex, Pawn->GetHandComponent()->GetCardNum(), OnCardMovementEnd);
	UpdateMaterials();
}

void ACCCard::Select(ACCPlayerPawn* Pawn)
{
	//TODO: Check if In Planning Mode
	bool bToSelected = true;
	if(CurrentCardState == ECardState::Played)
	{
		bToSelected = false;
		CurrentCardState = ECardState::Hovered;
		Pawn->GetPlayedCardIndex().Remove(CardIndex);

		FOnCardMovementEnd OnCardMovementEnd;
		CardMovement->StartMovement(CardIndex, Pawn->GetHandComponent()->GetCardNum(), OnCardMovementEnd);
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
	
	FOnCardMovementEnd OnCardMovementEnd;
	CardMovement->StartMovement(CardIndex, Pawn->GetHandComponent()->GetCardNum(), OnCardMovementEnd);
	UpdateMaterials();
	OnSelectCardEffects(bToSelected, Pawn);
}

void ACCCard::UnSelect(ACCPlayerPawn* Pawn)
{
	//TODO: Check if In Planning Mode
	CurrentCardState = ECardState::Hovered;
	Pawn->SetCurrentSelectedCardIndex(-1);
	FOnCardMovementEnd OnCardMovementEnd;
	CardMovement->StartMovement(CardIndex, Pawn->GetHandComponent()->GetCardNum(), OnCardMovementEnd);
	UpdateMaterials();
	OnSelectCardEffects(false, Pawn);
}

