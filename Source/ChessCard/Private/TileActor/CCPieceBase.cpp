// Fill out your copyright notice in the Description page of Project Settings.


#include "TileActor/CCPieceBase.h"

ACCPieceBase::ACCPieceBase(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
}

