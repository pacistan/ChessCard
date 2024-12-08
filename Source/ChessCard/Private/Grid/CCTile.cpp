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
	if(!IsValid(MeshComponent->GetStaticMesh()))
	{
		ConstructorHelpers::FObjectFinder<UStaticMesh> MeshRef(TEXT("/Engine/BasicShapes/Plane.Plane"));
		MeshComponent->SetStaticMesh(MeshRef.Object);
		MaterialInstance = UMaterialInstanceDynamic::Create(MeshComponent->GetMaterial(0), this);
		MeshComponent->SetMaterial(0, MaterialInstance);
	}
}

void ACCTile::SetHighlight(bool bIsHighlight, FOnClickTileDelegate OnClickDelegate)
{
	if(IsHighlighted == bIsHighlight) return;
	IsHighlighted = bIsHighlight;
	FColor Color = bIsHighlight ? (IsHovered ?  HoveredColor : HighlightColor) : ColorMap[TileType];
	MaterialInstance->SetVectorParameterValue("Color", Color);
	//UMaterialInterface* Mat = bIsHighlight ? HighlightMaterial : nullptr;
	//MeshComponent->SetOverlayMaterial(Mat);
	OnClickEvent = OnClickDelegate;

	auto GridManager = GetGridManager(GetWorld());
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
		MaterialInstance->SetVectorParameterValue("Color", HoveredColor);
	}
}

void ACCTile::StopHover(ACCPlayerPawn* Player)
{
	IsHovered = false;

	FColor Color = IsHighlighted ? HighlightColor : ColorMap[TileType];
	if(MaterialInstance == nullptr)
	{
		DEBUG_ERROR("Material Instance of Tile %s is null", *GetName());
		return;
	}
	MaterialInstance->SetVectorParameterValue("Color", Color);
}

void ACCTile::BeginPlay()
{
	Super::BeginPlay();

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

	if(!IsValid(MaterialInstance))
	{
		MaterialInstance = UMaterialInstanceDynamic::Create(MeshComponent->GetMaterial(0), this);
		MeshComponent->SetMaterial(0, MaterialInstance);
	}
	
	if(!IsValid(MeshComponent) || !IsValid(MaterialInstance))
	{
		TileType = PreviousTileType;
		return;
	}
	MaterialInstance->SetVectorParameterValue("Color", ColorMap[TileType]);
	
	PreviousTileType = TileType;
}

void ACCTile::Test()
{
	BlueprintEditorTick(0);
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
