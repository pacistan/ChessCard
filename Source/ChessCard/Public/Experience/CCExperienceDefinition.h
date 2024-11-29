// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CCExperienceDefinition.generated.h"

class UCCPawnData;

// TODO : add Decors to load ?
// TODO : add game Process to load (Deck, special rules etc...) ?
// TODO : List Actions to perform when experience is loaded (like load assets, load game process, etc...) ?

/**
 *  Definition of an experience
 */
UCLASS(BlueprintType, Const)
class CHESSCARD_API UCCExperienceDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()
	/* ------------------------------------------ MEMBERS -------------------------------------------*/
public:
	/** The default pawn class to spawn for players */
	UPROPERTY(EditDefaultsOnly, Category=Gameplay)
	TObjectPtr<const UCCPawnData> DefaultPawnData;

	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
public:
	UCCExperienceDefinition();
};
