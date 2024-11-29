﻿#include "Player/CCPlayerState.h"

#include "GameModes/CCGameMode.h"
#include "Player/CCPawnData.h"
#include "GameModes/CCGameState.h"
#include "GameModes/Component/CCExperienceManagerComponent.h"
#include "Macro/CCLogMacro.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"
#include "Player/CCPlayerController.h"


ACCPlayerState::ACCPlayerState(const FObjectInitializer& ObjectInitializer) : Super (ObjectInitializer)
{
	NetUpdateFrequency = 100.0f;
}

ACCPlayerController* ACCPlayerState::GetCCPlayerController() const
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

void ACCPlayerState::OnExperienceLoaded(const UCCExperienceDefinition* CurrentExperience)
{
	if (ACCGameMode* GameMode = GetWorld()->GetAuthGameMode<ACCGameMode>()) {
		if (const UCCPawnData* NewPawnData = GameMode->GetPawnDataForController(GetOwningController())) {
			SetPawnData(NewPawnData);
		} else {
			DEBUG_LOG("ALyraPlayerState::OnExperienceLoaded(): Unable to find PawnData to initialize player state [%s]!", *GetNameSafe(this));
		}
	}
}

void ACCPlayerState::BeginPlay()
{
	Super::BeginPlay();
}

void ACCPlayerState::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	UWorld* World = GetWorld();
	if (World && World->IsGameWorld() && World->GetNetMode() != NM_Client)
	{
		AGameStateBase* GameState = GetWorld()->GetGameState();
		check(GameState);
		UCCExperienceManagerComponent* ExperienceComponent = GameState->FindComponentByClass<UCCExperienceManagerComponent>();
		check(ExperienceComponent);
		ExperienceComponent->CallOrRegister_OnExperienceLoaded(FOnCCExperienceLoaded::FDelegate::CreateUObject(this, &ThisClass::OnExperienceLoaded));
	}
}
