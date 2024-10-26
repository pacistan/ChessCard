#include "Grid/CCTile.h"

#include "Player/CCPlayerPawn.h"

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


void ACCTile::SetHighlight(bool bIsHighlight)
{
	IsHighlighted = bIsHighlight;
	FColor Color = bIsHighlight ? (IsHovered ?  HoveredColor : HighlightColor) : ColorMap[TileType];
	MaterialInstance->SetVectorParameterValue("Color", Color);
}


void ACCTile::Click(ACCPlayerPawn* Player)
{
	if(IsHighlighted)
	{
		Player->PlaySelectedCard(this);
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
