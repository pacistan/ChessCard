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
#include "TileActor/CCSplineMeshActor.h"


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
	BPE_OnUnitDestroy(InitilizationProperties.IsPreview);
	if(!GetIsPreview())
	{
		DEBUG_LOG("");
	}
	Destroy();
}

void ACCPieceBase::MLC_DestroyPiece_Implementation()
{
	DestroyPiece();
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
	bIsPreview = InitilizationProperties.IsPreview;

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
	BPE_ConstructTile(CardDataRowHandle, GetGridManager(GetWorld())->GetTile(CurrentCoordinates)->GetMaterialFromMap(TileType), InitilizationProperties.IsPreview);
	SetActorRotation(FRotator());

	//SetActorScale3D(FVector::One());
}

void ACCPieceBase::CreateSpline(TArray<FVector> Positions, TArray<AActor*>& ActionVisuals)
{
	FActorSpawnParameters SpawnParams;
	FVector Location = GetActorLocation() / 2; 
	
	ACCSplineMeshActor* Spline = GetWorld()->SpawnActor<ACCSplineMeshActor>(SplineClass, Location, FRotator(), SpawnParams);
	SplineComponent = Spline->SplineComponent;
	ActionVisuals.Add(Spline);
	//ClearSpline();
	
	SplineComponent->AddSplinePoint(GetActorLocation() / 2, ESplineCoordinateSpace::World);
	SplineComponent->SetSplinePointType(SplineComponent->GetNumberOfSplinePoints() - 1, ESplinePointType::Linear);
	for (const auto& Position : Positions)
	{
		SplineComponent->AddSplinePoint(Position - GetActorLocation() / 2, ESplineCoordinateSpace::World);
		//SplineComponent->SetSplinePointType(SplineComponent->GetNumberOfSplinePoints() - 1, ESplinePointType::Linear);
	}

	SplineComponent->UpdateSpline();
	SetSplinePoints();
}

void ACCPieceBase::SetSplinePoints()
{
	FVector InitialTangent = FVector();
	if (SplineComponent->GetNumberOfSplinePoints() >= 2)
	{
		FVector FirstPoint = SplineComponent->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
		FVector SecondPoint = SplineComponent->GetLocationAtSplinePoint(1, ESplineCoordinateSpace::World);
		InitialTangent = (SecondPoint - FirstPoint).GetSafeNormal() * 100.0f;
		SplineComponent->SetTangentAtSplinePoint(0, InitialTangent, ESplineCoordinateSpace::World, true);
	}
	
	// Create a spline mesh component for each segment of the spline
	for (int32 i = 2; i < SplineComponent->GetNumberOfSplinePoints() - 1; ++i)
	{
		USplineMeshComponent* SplineMeshComponent = NewObject<USplineMeshComponent>(SplineComponent);
		SplineMeshComponent->RegisterComponent();
		SplineMeshComponent->SetMobility(EComponentMobility::Movable);
		SplineMeshComponent->AttachToComponent(SplineComponent, FAttachmentTransformRules::KeepRelativeTransform);
		SplineMeshComponent->SetStaticMesh(SplineMesh);
		SplineMeshComponent->SetMaterial(0, SplineMaterial);
		SplineMeshComponent->SetStartScale(.03f * FVector2D::One());
		SplineMeshComponent->SetEndScale(.03f * FVector2D::One());
		// Set the spline points for this segment
		FVector Start = SplineComponent->GetLocationAtSplinePoint(i, ESplineCoordinateSpace::World);
		FVector StartTangent;
		if(i == 0)
		{
			StartTangent = InitialTangent;
		}
		else
		{
			StartTangent = SplineComponent->GetTangentAtSplinePoint(i, ESplineCoordinateSpace::World);
		}
		FVector End = SplineComponent->GetLocationAtSplinePoint(i + 1, ESplineCoordinateSpace::World);
		FVector EndTangent = SplineComponent->GetTangentAtSplinePoint(i + 1, ESplineCoordinateSpace::World);

		SplineMeshComponent->SetStartAndEnd(Start, StartTangent, End, EndTangent);
		SplineMeshComponents.Add(SplineMeshComponent);
		// Other settings for the spline mesh (material, etc.) can be configured here
	}
}

void ACCPieceBase::ClearSpline()
{
	SplineComponent->ClearSplinePoints();
	for(int i = SplineMeshComponents.Num() - 1; i >= 0; i--)
	{
		SplineMeshComponents[i]->DestroyComponent();
	}
	SplineMeshComponents.Empty(true);
}

void ACCPieceBase::OnRep_InitProperties()
{	
	if(!IsInitialized)
	{
		InternalInit();
	}
}

void ACCPieceBase::MLC_OnDivineCounterChange_Implementation()
{
	BPE_OnDivineCounterChange();
}

void ACCPieceBase::RPC_SetVisible_Implementation()
{
	DEBUG_ERROR("SET VISIBLE");
	SetActorHiddenInGame(false);
	SetActorEnableCollision(true);
}

void ACCPieceBase::UnSelect()
{
	if(IsSelected)
	{
		GetGridManager(GetWorld())->UnhighlightTiles();
		IsSelected = false;
	}
}
