#include "TileActor/CCTileUnit.h"

#include "MeshCardBuild.h"
#include "TileActor/PatternMapEndPoint.h"
#include "Card/CCCard.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Engine/SplineMeshActor.h"
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
	FIntPoint OutPoint = FIntPoint();
	for(auto Movement : PatternMovement)
	{
		EndPoint += Movement.Direction;
		if(Movement.MovementType == EMovementType::Stoppable)
		{
			OutPoint = EndPoint;
		}
	}
	return OutPoint;
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
		MinotaurHighlightDestinationTiles();
		return;
	}

	auto TargetPatternList = CardData.DivineAngerTriggerNumber <= DivineAngerCounter ? DivineAngerPatternList : PatternList;
	
	for(auto& MvtData : TargetPatternList)
	{
		FIntPoint ProgressCoordinates = CurrentCoordinates;
		bool CanContinue = false;
		for(int i = 0; i < MvtData.Num(); i++)
		{
			ProgressCoordinates += MvtData[i].Direction; 
			if(MvtData[i].MovementType == EMovementType::Stoppable)
			{
				ACCTile* CoordinateTile =  GridManager->GetTile(ProgressCoordinates);
				if(IsValid(CoordinateTile) && CoordinateTile->IsAccessibleForTeam(Pawn->GetPlayerState<ACCPlayerState>()->GetTeam()))
				{
					CanContinue = true;
					break;
				}
			}
		}
		if(!CanContinue)
		{
			continue;
		}

		ProgressCoordinates = CurrentCoordinates;
		for(auto& Coordinate : MvtData)
		{
			ProgressCoordinates += Coordinate.Direction;
			ACCTile* Tile = GridManager->GetTile(ProgressCoordinates);
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
		int Counter = 1;

		
		while(IsValid(NextTile) && NextTile->IsAccessibleForTeam(Team) && Counter < 100)
		{
			CurrentTile->SetHighlight(true, OnClickTileDelegate, EHighlightMode::Effect);

			Counter++;
			CurrentTile = NextTile;
			NextTile = GridManager->GetTile(CurrentCoordinates + Directions[i] * (Counter + 1));
		}
		OnClickTileDelegate.BindDynamic(this, &ACCTileUnit::OnDestinationTileClicked);
		CurrentTile->SetHighlight(true, OnClickTileDelegate, EHighlightMode::Normal);
	}
}

void ACCTileUnit::OnDestinationTileClicked(ACCTile* Tile)
{
	FIntPoint EndPoint = FIntPoint(Tile->GetRowNum(), Tile->GetColumnNum());
	TArray<FPatternMapEndPoint> OutPatternMovement;
	IsMoved = true;
	FCardData CardData = *CardDataRowHandle.GetRow<FCardData>("");
	
	if(CardData.EffectType == EEffectType::Minotaur && CardData.DivineAngerTriggerNumber <= DivineAngerCounter)
	{
			FIntPoint Travel = EndPoint - CurrentCoordinates;
		int Magnitude = FMath::Abs(Travel.X) + FMath::Abs(Travel.Y);
		OutPatternMovement.Init(FPatternMapEndPoint(EMovementType::ApplyEffect, Travel / Magnitude), Magnitude);
		OutPatternMovement.Last().MovementType = EMovementType::Stoppable;
	}
	else
	{
		auto TargetPatternList = CardData.DivineAngerTriggerNumber <= DivineAngerCounter ? DivineAngerPatternList : PatternList;
		int StopIndex = 0;
		bool IsPatternFound = false;
		for(auto& PatternMovement : TargetPatternList)
		{
			FIntPoint Progress = CurrentCoordinates;
			for(int i = 0; i < PatternMovement.Num(); i++)
			{
				Progress += PatternMovement[i].Direction;
				if(Progress == EndPoint)
				{
					if(PatternMovement[i].MovementType == EMovementType::Stoppable)
					{
						OutPatternMovement = PatternMovement;
						StopIndex = i;
						IsPatternFound = true;
					}
					break;
				}
			}
			if(OutPatternMovement.IsEmpty())
			{
				continue;
			}

			for(int i = OutPatternMovement.Num() - 1; i >= 0; i--)
			{
				if(i > StopIndex && OutPatternMovement[i].MovementType != EMovementType::ApplyEffect)
				{
					OutPatternMovement.RemoveAt(i);
				}
				else if(i < StopIndex && OutPatternMovement[i].MovementType == EMovementType::Stoppable)
				{
					OutPatternMovement[i].MovementType = EMovementType::Normal;
				}
			}
			if(IsPatternFound)
			{
				break;
			}
		}
	}
	
	TArray<AActor*> MovementVisualActors;
	MovementVisualActors.Reserve(OutPatternMovement.Num());
	if(IsValid(MovementPointActorClass) && IsValid(DestinationPointActorClass) && IsValid(EffectPointActorClass))
	{
		FIntPoint ProgressPoint = CurrentCoordinates;
		FActorSpawnParameters SpawnParameters;
		
		/*TArray<USplineMeshComponent*> SplineMeshComponents;
		MovementVisualActors.Add(GetWorld()->SpawnActor<AActor>(VisualMovementSplineSubclass, FVector(), FRotator(), SpawnParameters));
		ASplineMeshActor* SplineMeshActor = Cast<ASplineMeshActor>(MovementVisualActors.Last());*/

		FVector PreviousPosition = GetGridManager(GetWorld())->CoordinatesToPosition(CurrentCoordinates) + FVector::UpVector * 20;
		for(int i = 0; i < OutPatternMovement.Num(); i++)
		{
			ProgressPoint += OutPatternMovement[i].Direction;
			auto LastTile = GetGridManager(GetWorld())->GetTile(ProgressPoint);
			if(!IsValid(LastTile) || !LastTile->IsAccessibleForTeam(Team)) continue;
			FVector PointPosition =  GetGridManager(GetWorld())->CoordinatesToPosition(ProgressPoint) + FVector::UpVector * 20;
			TSubclassOf<AActor> Subclass = OutPatternMovement[i].MovementType == EMovementType::Stoppable && Tile == LastTile ? DestinationPointActorClass :
				OutPatternMovement[i].MovementType == EMovementType::ApplyEffect ? EffectPointActorClass : MovementPointActorClass;
			FVector Direction = FVector(OutPatternMovement[i].Direction.X, OutPatternMovement[i].Direction.Y, 0);
			FRotator Rotator = UKismetMathLibrary::MakeRotFromXZ(Direction, FVector::UpVector);
			MovementVisualActors.Add(GetWorld()->SpawnActor<AActor>(Subclass, PointPosition, Rotator, SpawnParameters));
			
			/*SplineMeshComponents.Add(NewObject<USplineMeshComponent>(SplineMeshActor, NAME_None, RF_Transactional));
			SplineMeshComponents[i]->SetForwardAxis(ESplineMeshAxis::X, false);
			SplineMeshComponents[i]->SetStaticMesh(SplineStaticMesh);
			SplineMeshComponents[i]->SetStartAndEnd(PreviousPosition, FVector(),
				PointPosition, FVector(), false);
			SplineMeshComponents[i]->RegisterComponent();
			
			SplineMeshComponents[i]->SetMaterial(0, SplineMaterial);*/
		}
	}
	LinkedCard->MoveUnit(Tile, OutPatternMovement, MovementVisualActors, this);
}

void ACCTileUnit::SetHighlight(bool bToHighlight, ETeam InTeam, FOnClickUnitDelegate InOnClickDelegate,
	FOnHoverUnitDelegate InOnHoverUnitDelegate)
{
	if(bToHighlight && (IsMoved || InTeam != Team || IsStunned)) return;
	IsHighlighted = bToHighlight;
	UMaterialInterface* NewMaterial = IsHighlighted ? HighlightMat : nullptr;
	BPE_UpdateMaterial(NewMaterial);
	//MeshComponent->SetMaterial(0, NewMaterial);
}

void ACCTileUnit::StartHover(ACCPlayerPawn* Player)
{
	ACCTile* Tile = GetGridManager(GetWorld())->GetTile(CurrentCoordinates);
	if(Tile->GetIsHighlighted())
	{
		Tile->StartHover(Player);
		return;
	}
	else if(Player->GetSelectedUnit() != nullptr)
	{
		return;
	}
	
	if(IsHighlighted)
	{
		HighlightDestinationTiles(Player);
	}
}

void ACCTileUnit::StopHover(ACCPlayerPawn* Player)
{
	ACCTile* Tile = GetGridManager(GetWorld())->GetTile(CurrentCoordinates);
	if(Tile->GetIsHighlighted() || Player->GetSelectedUnit() != nullptr)
	{
		return;
	}
	
	if(!IsSelected && IsHighlighted)
	{
		GetGridManager(GetWorld())->UnhighlightTiles();
	}
}

void ACCTileUnit::Click(ACCPlayerPawn* Player)
{
	ACCTile* CurrentTile = GetGridManager(GetWorld())->GetTile(CurrentCoordinates);
	if(CurrentTile->GetIsHighlighted())
	{
		CurrentTile->Click(Player);
		return;
	}
	if(IsHighlighted)
	{
		IsSelected = true;
		//MeshComponent->SetMaterial(0, SelectedMaterial);
		BPE_UpdateMaterial(SelectedMaterial);
		TArray<AActor*> MovementVisualActors;
		if(CardDataRowHandle.GetRow<FCardData>("")->EffectType == EEffectType::Embrasement)
		{
			LinkedCard = Player->GetHandComponent()->Cards[Player->GetCurrentSelectedCardIndex()];
			FActorSpawnParameters SpawnParameters;
			for(int i = -1; i <= 1; i++)
			{
				for(int j = -1; j <= 1; j++)
				{
					if(i == 0 && j == 0) continue;
					int X = CurrentCoordinates.X + i;
					int Y = CurrentCoordinates.Y + j;
					ACCTile* Tile = GetGridManager(GetWorld())->GetTile(FIntPoint(X, Y));
					if(IsValid(Tile) && (Tile->GetTileType() == ETileType::Normal || Tile->GetTileType() == ETileType::BonusTile || Tile->GetTileType() == ETileType::ScoreTile))
					{
						FVector PointPosition =  Tile->GetActorLocation() + FVector::UpVector * 20;
						MovementVisualActors.Add(GetWorld()->SpawnActor<AActor>(EffectPointActorClass, PointPosition, FRotator::ZeroRotator, SpawnParameters)); 
					}
				}
			}
			LinkedCard->TriggerEmbrasement(GetGridManager(GetWorld())->GetTile(CurrentCoordinates), MovementVisualActors,this);
		}
		else
		{
			Player->SetSelectedUnit(this);
		}
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
	DOREPLIFETIME(ACCTileUnit, IsMoved);
}

void ACCTileUnit::UnSelect()
{
	Super::UnSelect();
	if(IsSelected)
	{
		BPE_UpdateMaterial(nullptr);
		//MeshComponent->SetMaterial(0, BaseMaterial);
	}
}



