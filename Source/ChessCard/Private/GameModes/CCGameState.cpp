﻿#include "GameModes/CCGameState.h"
#include "EngineUtils.h"
#include "Engine/PlayerStartPIE.h"
#include "Macro/CCLogMacro.h"
#include "Grid/CCGridManager.h"
#include "Grid/CCTile.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/CCPlayerController.h"
#include "Player/CCPlayerPawn.h"
#include "Player/CCPlayerStart.h"
#include "Player/CCPlayerState.h"

extern ENGINE_API float GAverageFPS;

ACCGameState::ACCGameState(const FObjectInitializer& ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

void ACCGameState::BeginPlay()
{
	Super::BeginPlay();
	GridManager = Cast<ACCGridManager>(UGameplayStatics::GetActorOfClass(GetWorld(), ACCGridManager::StaticClass()));
}

bool ACCGameState::AddPlayerController(ACCPlayerController* PlayerController)
{
	if (!IsValid(PlayerController)) {
		return false;
	}
	
	return PlayerControllers.AddUnique(PlayerController) != INDEX_NONE;
}

void ACCGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
    Super::GetLifetimeReplicatedProps(OutLifetimeProps);
    DOREPLIFETIME(ACCGameState, GridManager);
}

ACCPlayerState* ACCGameState::GetPlayerStateOfTeam(ETeam Team) const
{
	for (auto PlayerState : PlayerArray) {
		if (ACCPlayerState* CCPlayerState = Cast<ACCPlayerState>(PlayerState)) {
			if (CCPlayerState->GetTeam() == Team) {
				return CCPlayerState;
			}
		}
	}

	return nullptr;
}
