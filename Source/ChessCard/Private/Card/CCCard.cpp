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
		UE_LOG(LogTemp, Error, TEXT("Pawn has an invalid Player Index"));
		break;
	}

	bool toHighlight = bIsSelected;
	FTileTypeDelegate TileTypeDelegate;
	auto Lambda = [&toHighlight](ACCTile* Tile){Tile->SetHighlight(toHighlight);};
	TileTypeDelegate.BindLambda(Lambda);
	Cast<ACCGameState>(UGameplayStatics::GetGameState(GetWorld()))->GetGridManager()->ApplyLambdaToTileType(TargetTileType, TileTypeDelegate);
}

void ACCCard::BeginPlay()
{
	Super::BeginPlay();
}

void ACCCard::Play(ACCPlayerPawn* Pawn)
{
	if(!CardMovement->IsInterruptable)
	{
		return;
	}
	CurrentCardState = ECardState::Played;
	CardMovement->StartMovement(CardIndex, Pawn->GetHandComponent()->GetCardNum());
	UpdateMaterials();
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
	CurrentCardState = ECardState::Inactive;
	Pawn->SetCurrentSelectedCardIndex(-1);
	CardMovement->StartMovement(CardIndex, Pawn->GetHandComponent()->GetCardNum());
	UpdateMaterials();
	OnSelectCardEffects(false, Pawn);
}

