#include "Player/CCPlayerState.h"

#include "GameModes/CCGameMode.h"
#include "GameModes/CCGameState.h"
#include "Macro/CCLogMacro.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Player/CCPlayerController.h"


ACCPlayerState::ACCPlayerState(const FObjectInitializer& ObjectInitializer) : Super (ObjectInitializer)
{
	
}

void ACCPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void ACCPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACCPlayerState, Team);
}
