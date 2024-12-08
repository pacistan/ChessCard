// Fill out your copyright notice in the Description page of Project Settings.


#include "Interfaces/CCGridManagerInterface.h"

#include "GameModes/CCGameState.h"
#include "Kismet/GameplayStatics.h"
#include "Macro/CCLogMacro.h"


// Add default functionality here for any ICCGridManagerInterface functions that are not pure virtual.
ACCGridManager* ICCGridManagerInterface::GetGridManager(UWorld* World)
{
	if(!IsValid(World))
	{
		DEBUG_ERROR("NO WORLD when trying to retrieve Grid Manager");
		return nullptr;
	}
	ACCGameState* GameState = Cast<ACCGameState>(World->GetGameState());
	if(!IsValid(GameState))
	{
		DEBUG_ERROR("NO GAMESTATE when trying to retrieve Grid Manager");
		return nullptr;
	}
	
	return GameState->GetGridManager();
}
