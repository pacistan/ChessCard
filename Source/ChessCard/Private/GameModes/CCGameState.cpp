#include "GameModes/CCGameState.h"
#include "EngineUtils.h"
#include "Engine/PlayerStartPIE.h"
#include "Macro/CCLogMacro.h"
#include "Grid/CCGridManager.h"
#include "Grid/CCTile.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Player/CCPlayerPawn.h"
#include "Player/CCPlayerStart.h"

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
