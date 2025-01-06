#include "TileActor/CCPieceBase.h"

#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "GameModes/CCGameState.h"
#include "Grid/CCGridManager.h"
#include "Grid/CCTile.h"
#include "Macro/CCLogMacro.h"
#include "Net/UnrealNetwork.h"
#include "Player/CCPlayerState.h"


ACCPieceBase::ACCPieceBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComponent;

	EffectContainerWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("EffectContainerWidget"));
	EffectContainerWidget->SetupAttachment(RootComponent);

}

void ACCPieceBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACCPieceBase, InitilizationProperties);
	DOREPLIFETIME(ACCPieceBase, CurrentCoordinates);
	DOREPLIFETIME(ACCPieceBase, PreviousCoordinates);
}

void ACCPieceBase::DestroyPiece()
{
	GetGridManager(GetWorld())->GetTile(CurrentCoordinates)->RemovePiece(this);
	BPE_OnUnitDestroy();
	Destroy();
}

void ACCPieceBase::MLC_DestroyPiece_Implementation()
{
	GetGridManager(GetWorld())->GetTile(CurrentCoordinates)->RemovePiece(this);
	BPE_OnUnitDestroy();
	Destroy();
}

void ACCPieceBase::InitUnit(const FInitilizationProperties& InInitilizationProperties)
{
	InitilizationProperties = InInitilizationProperties;
	IsInitialized = true;
	InternalInit();
}

void ACCPieceBase::InternalInit()
{
	CurrentCoordinates = InitilizationProperties.Coordinates;
	CardDataRowHandle = InitilizationProperties.CardDataRowHandle;
	Team = InitilizationProperties.Team;
	UnitGuid = InitilizationProperties.InstanceID;
	GetGridManager(GetWorld())->GetTile(CurrentCoordinates)->AddPiece(this);
	IsInitialized = true;

	ETileType TileType = ETileType::Player1;
	switch(Team)
	{
	case ETeam::None:
		break;
	case ETeam::One:
		break;
	case ETeam::Two:
		TileType = ETileType::Player2;
		break;
	case ETeam::Three:
		TileType = ETileType::Player3;
		break;
	case ETeam::Four:
		TileType = ETileType::Player4;
		break;
	}
	
	BPE_ConstructTile(*CardDataRowHandle.GetRow<FCardData>(""), GetGridManager(GetWorld())->GetTile(CurrentCoordinates)->GetMaterialFromMap(TileType));
	SetActorRotation(FRotator());

	//SetActorScale3D(FVector::One());
}

void ACCPieceBase::CreateSpline(TArray<FVector> Positions)
{
	PathSpline->ClearSplinePoints();
	for(int i = SplineMeshComponents.Num() - 1; i >= 0; i--)
	{
		SplineMeshComponents[i]->DestroyComponent();
	}
	SplineMeshComponents.Empty(true);
	
	bool bIsFirstPosition = true;
	for (const auto& Position : Positions)
	{
		if(bIsFirstPosition)
		{
			bIsFirstPosition = false;
			PathSpline->AddSplinePoint(Position, ESplineCoordinateSpace::World, true);
			continue;
		}
		/*if(Position != GetCurrentTile())
		{
			FVector location = FVector(Position->GetActorLocation() + FVector::UpVector * 40);
			PathSpline->AddSplinePoint(location, ESplineCoordinateSpace::World, true);
		}*/
	}
	PathSpline->AddSplinePoint( SplineStartPoint->GetComponentLocation() ,ESplineCoordinateSpace::World, true);
	SetSplinePoints();
}

void ACCPieceBase::SetSplinePoints()
{
	// Create a spline mesh component for each segment of the spline
	for (int32 i = 0; i < PathSpline->GetNumberOfSplinePoints() - 1; ++i)
	{
		USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(this);
		SplineMeshComponent->RegisterComponent();
		SplineMeshComponent->SetMobility(EComponentMobility::Movable);
		SplineMeshComponent->AttachToComponent(PathSpline, FAttachmentTransformRules::KeepRelativeTransform);
		SplineMeshComponent->SetStaticMesh(SplineMesh);
		SplineMeshComponent->SetMaterial(0, SplineMaterial);
		SplineMeshComponent->SetStartScale(.03f * FVector2D::One());
		SplineMeshComponent->SetEndScale(.03f * FVector2D::One());
		// Set the spline points for this segment
		FVector Start = PathSpline->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
		FVector StartTangent = PathSpline->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::World);
		FVector End = PathSpline->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::World);
		FVector EndTangent = PathSpline->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::World);

		SplineMeshComponent->SetStartAndEnd(Start, StartTangent, End, EndTangent);
		SplineMeshComponents.Add(SplineMeshComponent);
		// Other settings for the spline mesh (material, etc.) can be configured here
	}
}

void ACCPieceBase::OnRep_InitProperties()
{	
	if(!IsInitialized)
	{
		InternalInit();
	}
}

void ACCPieceBase::UnSelect()
{
	if(IsSelected)
	{
		GetGridManager(GetWorld())->UnhighlightTiles();
		IsSelected = false;
	}
}