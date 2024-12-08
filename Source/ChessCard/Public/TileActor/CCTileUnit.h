// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CCPieceBase.h"
#include "CCTileUnit.generated.h"

/**
 *  Tile Actor that Have a the ability to move 
 */
UCLASS()
class CHESSCARD_API ACCTileUnit : public ACCPieceBase
{
	GENERATED_BODY()
	/* ------------------------------------------ MEMBERS -------------------------------------------*/
public:
	
	/* ----------------------------------------- FUNCTIONS -------------------------------------------*/
public:
	ACCTileUnit(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
};
