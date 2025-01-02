#include "TileActor/CCPieceBase.h"

#include "Grid/CCGridManager.h"
#include "Grid/CCTile.h"
#include "Net/UnrealNetwork.h"

ACCPieceBase::ACCPieceBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComponent;

}

void ACCPieceBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACCPieceBase, CurrentCoordinates);
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

void ACCPieceBase::InitUnit(FIntPoint StartCoordinates, ETeam InTeam, const TArray<FUnitMovementData>& InPattern, const FGuid& NewGuid, FDataTableRowHandle InCardDataRowHandle)
{
	CurrentCoordinates = StartCoordinates;
	CardDataRowHandle = InCardDataRowHandle;
	Team = InTeam;
	UnitGuid = NewGuid;
	GetGridManager(GetWorld())->GetTile(CurrentCoordinates)->AddPiece(this);
}

void ACCPieceBase::MLC_InitUnit_Implementation(FIntPoint StartCoordinates, ETeam InTeam,
	const TArray<FUnitMovementData>& InPattern, const FGuid& NewGuid, FDataTableRowHandle InCardDataRowHandle)
{
	InitUnit(StartCoordinates, InTeam, InPattern, NewGuid, InCardDataRowHandle);
}
