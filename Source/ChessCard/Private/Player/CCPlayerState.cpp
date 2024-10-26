// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CCPlayerState.h"

#include "GameModes/CCGameMode.h"
#include "Player/CCPawnData.h"
#include "GameModes/CCGameState.h"
#include "Macro/CCLogMacro.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Player/CCPlayerController.h"


ACCPlayerState::ACCPlayerState(const FObjectInitializer& ObjectInitializer) : Super (ObjectInitializer)
{
	NetUpdateFrequency = 100.0f;
}

ACCPlayerController* ACCPlayerState::GetCGPlayerController() const
{
	return Cast<ACCPlayerController>(GetOwner());
}

void ACCPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	FDoRepLifetimeParams SharedParams;
	SharedParams.bIsPushBased = true;

	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, PawnData, SharedParams);
}

void ACCPlayerState::OnRep_PawnData()
{
	
}

void ACCPlayerState::SendPlayerIndexToClients_Implementation(int32 PlayerIndex)
{
}

void ACCPlayerState::AskServerToSendPlayerIndexToPawn_Implementation()
{
	SendPlayerIndexToClients();
}

void ACCPlayerState::SetPawnData(const UCCPawnData* InPawnData)
{
	check(InPawnData);
	if (!HasAuthority()) {
		return;
	}
	
	if (PawnData) {
		DEBUG_ERROR("Trying to set PawnData [%s] on player state [%s] that already has valid PawnData [%s].", *GetNameSafe(InPawnData), *GetNameSafe(this), *GetNameSafe(PawnData));
		return;
	}
	
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, PawnData, this);
	PawnData = InPawnData;
	
	ForceNetUpdate();
}

void ACCPlayerState::BeginPlay()
{
	Super::BeginPlay();
	AskServerToSendPlayerIndexToPawn();
}


void ACCPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	UWorld* World = GetWorld();
	if (World && World->IsGameWorld() && World->GetNetMode() != NM_Client) {
		AGameStateBase* GameState = GetWorld()->GetGameState();
		check(GameState);
		if (ACCGameMode* CCGameMode = GetWorld()->GetAuthGameMode<ACCGameMode>()) {
			if (const UCCPawnData* NewPawnData = CCGameMode->GetPawnDataForController(GetOwningController())) {
				SetPawnData(NewPawnData);
			} else {
				DEBUG_ERROR("ACGPlayerState::PostInitializeComponents(): Unable to find PawnData to initialize player state [%s]!", *GetNameSafe(this));
			}
		}
	}
	
}
