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
	CCRootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	RootComponent = CCRootComponent;
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>("Tile Mesh");
	MeshComponent->SetupAttachment(CCRootComponent);
}

void ACCTile::SetHighlight(bool bIsHighlight, FOnClickTileDelegate OnClickDelegate, EHighlightMode HighlightMode)
{
	if(IsHighlighted == bIsHighlight && HighlightMode == CurrentHighlightMode)
	{
		return;
	}
	IsHighlighted = bIsHighlight;

	UMaterialInterface* Material = bIsHighlight ? (IsHovered ?  HoveredMaterial : HighlightMaterial[HighlightMode]) : /*MaterialMap[TileType]*/nullptr;
	MeshComponent->SetOverlayMaterial(/*0, */Material); 
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
	if(IsHighlighted && CurrentHighlightMode == EHighlightMode::Normal)
	{
		OnClickEvent.ExecuteIfBound(this);
		Player->PlaySelectedCard(this);
		BPE_OnClickTile();
	}
}

void ACCTile::StartHover(ACCPlayerPawn* Player)
{
	IsHovered = true;
	if(IsHighlighted && CurrentHighlightMode == EHighlightMode::Normal)
	{
		MeshComponent->SetOverlayMaterial(/*0, */HoveredMaterial);
	}
}

void ACCTile::StopHover(ACCPlayerPawn* Player)
{
	IsHovered = false;

	UMaterialInterface* Material = IsHighlighted ? HighlightMaterial[CurrentHighlightMode] :/* MaterialMap[TileType]*/nullptr;
	MeshComponent->SetOverlayMaterial(/*0, */Material);
}

void ACCTile::BeginPlay()
{
	Super::BeginPlay();
	BaseMaterial = MeshComponent->GetMaterial(0);
	PrimaryActorTick.SetTickFunctionEnable(false);
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

void ACCTile::UpdateMaterial()
{
	MeshComponent->SetMaterial(0, MaterialMap[TileType]);
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

void ACCTile::AddPiece(ACCPieceBase* Piece)
{
	if(HasAuthority())
	{
		DEBUG_LOG("Server Add Piece of Team  %s", *UEnum::GetValueAsName(Piece->Team).ToString());
	}else
	{
		DEBUG_LOG("Client Add Piece of Team  %s", *UEnum::GetValueAsName(Piece->Team).ToString());
	}
	if(Pieces.IsEmpty())
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

void ACCTile::MLC_AddPiece_Implementation(ACCPieceBase* Piece)
{
	AddPiece(Piece); 
}

void ACCTile::RemovePiece(ACCPieceBase* Piece)
{
	if(HasAuthority())
	{
		DEBUG_LOG("Server Remove Piece of Team  %s", *UEnum::GetValueAsName(Piece->Team).ToString());
	}else
	{
		DEBUG_LOG("Client Remove Piece of Team  %s", *UEnum::GetValueAsName(Piece->Team).ToString());
	}
	
	if(!Pieces.Contains(Piece))
	{
		DEBUG_ERROR("Piece %s already in Tile %s", *Piece->GetName(), *GetName());
		return;
	}
	Pieces.Remove(Piece);
	if(Pieces.IsEmpty())
	{
		GetGridManager(GetWorld())->UnregisterTileAsType(FIntPoint(GetRowNum(), GetColumnNum()), ETileType::Unit);
	}
}

void ACCTile::MLC_RemovePiece_Implementation(ACCPieceBase* Piece)
{
	RemovePiece(Piece);
}

bool ACCTile::IsAccessibleForTeam(ETeam Team)
{
	bool Result;
	switch(TileType)
	{
	case ETileType::Disabled:
		Result = false;
		break;
	case ETileType::Player1:
		Result = Team == ETeam::One ? true : false;
		break;
	case ETileType::Player2:
		Result = Team == ETeam::Two ? true : false;
		break;
	case ETileType::Player3:
		Result = Team == ETeam::Three ? true : false;
		break;
	case ETileType::Player4:
		Result = Team == ETeam::Four ? true : false;
		break;
	default:
		Result = true;
	}
	return Result;
}

ACCPieceBase* ACCTile::GetPiece(FGuid TargetID)
{
	for(auto Piece : Pieces)
	{
		if(Piece->UnitGuid == TargetID)
		{
			return Piece;
		}
	}

	return nullptr;
}
