﻿#include "Player/CCPlayerState.h"

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

void ACCPlayerState::SetEndTurn(bool bEndTurn)
{
	bHasEndedTurn = bEndTurn;
	if(!HasAuthority())
	{
		SRV_SetEndTurn(bHasEndedTurn);
	}
	else
	{
		EndTurnDelegate.ExecuteIfBound(this, bEndTurn);
	}
}

void ACCPlayerState::SRV_SetEndTurn_Implementation(bool bInEndTurn)
{
	bHasEndedTurn = bInEndTurn;
	EndTurnDelegate.ExecuteIfBound(this, bInEndTurn);
}

void ACCPlayerState::RPC_SetEndTurn_Implementation(bool bInEndTurn)
{
	SetEndTurn(bInEndTurn);
}

void ACCPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ACCPlayerState, Team);
	DOREPLIFETIME(ACCPlayerState, bHasEndedTurn);
	DOREPLIFETIME(ACCPlayerState, PlayerAvatar);
}
