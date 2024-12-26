// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CCPlayerStart.h"

#include "Player/CCPlayerState.h"


bool ACCPlayerStart::IsClaimed() const
{
	return ClaimingController != nullptr;
}

bool ACCPlayerStart::TryClaim(AController* OccupyingController)
{
	if (OccupyingController != nullptr && !IsClaimed()) {
		ClaimingController = OccupyingController;
		if (ACCPlayerState* ClaimingPlayerState = ClaimingController->GetPlayerState<ACCPlayerState>()) {
			if (Team != ETeam::None) {
				ClaimingPlayerState->SetTeam(Team);
			}
		}
		return true;
	}
	return false;
}
