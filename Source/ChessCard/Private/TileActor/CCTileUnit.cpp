#include "TileActor/CCTileUnit.h"

#include "MeshCardBuild.h"
#include "TileActor/PatternMapEndPoint.h"
#include "Card/CCCard.h"
#include "GameModes/CCEffectManagerComponent.h"
#include "GameModes/CCGameState.h"
#include "Grid/CCGridManager.h"
#include "Grid/CCTile.h"
#include "Hand/CCHandComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Net/UnrealNetwork.h"
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
	FCardData CardData = *CardDataRowHandle.GetRow<FCardData>(TEXT(""));
	auto GridManager = GetGridManager(GetWorld());
	check(GridManager);
	GridManager->GetTargetTiles(CardData.Pattern, PatternList);
	GridManager->GetTargetTiles(CardData.DivineAngerPattern, DivineAngerPatternList);
}

void ACCTileUnit::HighlightDestinationTiles(ACCPlayerPawn* Pawn)
{
	auto GridManager = GetGridManager(GetWorld());
	LinkedCard = Pawn->GetHandComponent()->Cards[Pawn->GetCurrentSelectedCardIndex()];
	check(LinkedCard);
	check(GridManager);
	check(Pawn);
	   
	GridManager->UnhighlightTiles();

	FCardData CardData = *CardDataRowHandle.GetRow<FCardData>("");
	if(CardData.EffectType == EEffectType::Minotaur && CardData.DivineAngerTriggerNumber <= DivineAngerCounter)
	{
		DEBUG_LOG("MINOTAUR!");
		MinotaurHighlightDestinationTiles();
		return;
	}
	
	
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

void ACCTileUnit::MinotaurHighlightDestinationTiles()
{
	ACCGridManager* GridManager = GetGridManager(GetWorld());
	TArray<FIntPoint> Directions {FIntPoint(0, 1), FIntPoint(0, -1), FIntPoint(1, 0), FIntPoint(-1, 0)};
	for(int i = 0; i < Directions.Num() ; i++)
	{
		FOnClickTileDelegate OnClickTileDelegate;
		ACCTile* CurrentTile = GridManager->GetTile(CurrentCoordinates + Directions[i]);
		ACCTile* NextTile = GridManager->GetTile(CurrentCoordinates + Directions[i] * 2);
		if(!IsValid(CurrentTile) || !CurrentTile->IsAccessibleForTeam(Team))
		{
			continue;
		}
		else if(!IsValid(NextTile)  || !NextTile->IsAccessibleForTeam(Team))
		{
			OnClickTileDelegate.BindDynamic(this, &ACCTileUnit::OnDestinationTileClicked);
			CurrentTile->SetHighlight(true, OnClickTileDelegate, EHighlightMode::Normal);
			continue;
		}
		else
		{
			CurrentTile->SetHighlight(true, OnClickTileDelegate, EHighlightMode::Effect);
		}
		int Counter = 1;

		
		while(IsValid(NextTile) && NextTile->IsAccessibleForTeam(Team) && Counter < 100)
		{
			Counter++;
			CurrentTile = NextTile;
			NextTile = GridManager->GetTile(CurrentCoordinates + Directions[i] * (Counter + 1));
			CurrentTile->SetHighlight(true, OnClickTileDelegate, EHighlightMode::Effect);
		}
		OnClickTileDelegate.BindDynamic(this, &ACCTileUnit::OnDestinationTileClicked);
		CurrentTile->SetHighlight(true, OnClickTileDelegate, EHighlightMode::Normal);
	}
}

void ACCTileUnit::OnDestinationTileClicked(ACCTile* Tile)
{
	FIntPoint EndPoint = FIntPoint(Tile->GetRowNum(), Tile->GetColumnNum());
	FIntPoint Travel = EndPoint - CurrentCoordinates;
	TArray<FPatternMapEndPoint> OutPatternMovement;
	IsMoved = true;
	for(auto PatternMovement : PatternList)
	{
		if(Travel == GetTravelRelativeCoordinates(PatternMovement))
		{
			OutPatternMovement = PatternMovement;
			break;
		}
	}
	
	TArray<AActor*> MovementVisualActors;
	MovementVisualActors.Reserve(OutPatternMovement.Num());
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
			MovementVisualActors.Add(GetWorld()->SpawnActor<AActor>(Subclass, PointPosition, Rotator, SpawnParameters)); 
		}
	}
	LinkedCard->MoveUnit(Tile, OutPatternMovement, MovementVisualActors, this);
}

void ACCTileUnit::SetHighlight(bool bToHighlight, ETeam InTeam, FOnClickUnitDelegate InOnClickDelegate,
	FOnHoverUnitDelegate InOnHoverUnitDelegate)
{
	if(bToHighlight && (IsMoved || InTeam != Team)) return;
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
	if(!IsSelected && IsHighlighted)
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

void ACCTileUnit::InternalInit()
{
	Super::InternalInit();
	SetTargetMap();
}

void ACCTileUnit::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACCTileUnit, IsStunned);
	DOREPLIFETIME(ACCTileUnit, DivineAngerCounter);
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
