#include "Grid/CCTile.h"

#include "GameModes/CCGameState.h"
#include "Grid/CCGridManager.h"
#include "Kismet/GameplayStatics.h"
#include "Macro/CCLogMacro.h"
#include "Player/CCPlayerPawn.h"
#include "TileActor/CCPieceBase.h"

ACCTile::ACCTile()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Tile Mesh");
	RootComponent = MeshComponent;
}

void ACCTile::SetHighlight(bool bIsHighlight, FOnClickTileDelegate OnClickDelegate,  EHighlightMode HighlightMode)
{
	if(IsHighlighted == bIsHighlight && HighlightMode == CurrentHighlightMode)
	{
		return;
	}
	IsHighlighted = bIsHighlight;

	UMaterialInterface* Material = bIsHighlight ? (IsHovered ?  HoveredMaterial : HighlightMaterial[HighlightMode]) : nullptr;
	MeshComponent->SetOverlayMaterial(Material);
	
	CurrentHighlightMode = HighlightMode;
	OnClickEvent = OnClickDelegate;
	  
	auto GridManager = GetGridManager(GetWorld());
	check(GridManager);
	if(IsHighlighted)
	{
		GridManager->RegisterTileAsType(FIntPoint(GetRowNum(), GetColumnNum()), ETileType::Highlighted);
	}
	else
	{
		GridManager->UnregisterTileAsType(FIntPoint(GetRowNum(), GetColumnNum()), ETileType::Highlighted);
	}
}

void ACCTile::Click(ACCPlayerPawn* Player)
{
	if(IsHighlighted)
	{
		Player->PlaySelectedCard(this);
		OnClickEvent.ExecuteIfBound(this);
	}
}

void ACCTile::StartHover(ACCPlayerPawn* Player)
{
	IsHovered = true;
	if(IsHighlighted)
	{
		MeshComponent->SetOverlayMaterial(HoveredMaterial);
	}
}

void ACCTile::StopHover(ACCPlayerPawn* Player)
{
	IsHovered = false;

	UMaterialInterface* Material = IsHighlighted ? HighlightMaterial[CurrentHighlightMode] : nullptr;
	MeshComponent->SetOverlayMaterial(Material);
}

void ACCTile::BeginPlay()
{
	Super::BeginPlay();
	BaseMaterial = MeshComponent->GetMaterial(0);
}

void ACCTile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
#if WITH_EDITOR
	if (GetWorld() != nullptr && GetWorld()->WorldType == EWorldType::Editor && PreviousTileType != TileType)
	{
		BlueprintEditorTick(DeltaTime);
	}
#endif
}

void ACCTile::BlueprintEditorTick(float DeltaTime)
{
	bool bIsHidden = TileType == ETileType::Disabled ? true : false;
	SetActorHiddenInGame(bIsHidden);

	if(!IsValid(MeshComponent) )
	{
		TileType = PreviousTileType;
		return;
	}

	if(!MaterialMap.Contains(TileType))
	{
		return;
	}
	
	UMaterialInterface* Material = MaterialMap[TileType];
	if(!IsValid(Material))
	{
		return;
	}
	
	MeshComponent->SetMaterial(0, Material);
	PreviousTileType = TileType;
}


bool ACCTile::ShouldTickIfViewportsOnly() const
{
	if (GetWorld() != nullptr && GetWorld()->WorldType == EWorldType::Editor)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void ACCTile::AddPieceLocal(ACCPieceBase* Piece)
{
	if(Pieces.Num() == 0)
	{
		GetGridManager(GetWorld())->RegisterTileAsType(FIntPoint(GetRowNum(), GetColumnNum()), ETileType::Unit);
	}
	if(Pieces.Contains(Piece))
	{
		DEBUG_ERROR("Piece %s already in Tile %s", *Piece->GetName(), *GetName());
		return;
	}
	Pieces.Add(Piece);
}

void ACCTile::RemovePiece(ACCPieceBase* Piece)
{
	Pieces.Add(Piece);
	if(!Pieces.Contains(Piece))
	{
		DEBUG_ERROR("Piece %s already in Tile %s", *Piece->GetName(), *GetName());
		return;
	}
	Pieces.Remove(Piece);
	if(Pieces.Num() == 0)
	{
		GetGridManager(GetWorld())->UnregisterTileAsType(FIntPoint(GetRowNum(), GetColumnNum()), ETileType::Unit);
	}
}
