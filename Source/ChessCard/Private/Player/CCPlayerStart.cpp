// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/CCPlayerStart.h"


bool ACCPlayerStart::IsClaimed() const
{
	return ClaimingController != nullptr;
}

bool ACCPlayerStart::TryClaim(AController* OccupyingController)
{
	if (OccupyingController != nullptr && !IsClaimed()) {
		ClaimingController = OccupyingController;
		return true;
	}
	return false;
}
