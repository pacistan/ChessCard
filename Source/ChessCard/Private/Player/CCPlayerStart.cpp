// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CCPlayerStart.h"

#include "Player/CCPlayerState.h"


bool ACCPlayerStart::IsClaimed() const
{
	return ClaimingController != nullptr;
}

bool ACCPlayerStart::TryClaim(AController* InController)
{
	if (InController != nullptr && !IsClaimed()) {
		ClaimingController = InController;
		if (ACCPlayerState* ClaimingPlayerState = ClaimingController->GetPlayerState<ACCPlayerState>()) {
			if (Team != ETeam::None) {
				ClaimingPlayerState->SetTeam(Team);
			}
		}
		OnPlayerStartClaimed(ClaimingController);
		return true;
	}
	return false;
}
