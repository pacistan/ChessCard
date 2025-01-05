#include "TileActor/CCPieceBase.h"

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
	Destroy();
}

void ACCPieceBase::MLC_DestroyPiece_Implementation()
{
	GetGridManager(GetWorld())->GetTile(CurrentCoordinates)->RemovePiece(this);
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
	ConstructTile(*CardDataRowHandle.GetRow<FCardData>(""));

	//SetActorScale3D(FVector::One());
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