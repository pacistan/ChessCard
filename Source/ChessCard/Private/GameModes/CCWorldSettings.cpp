// Fill out your copyright notice in the Description page of Project Settings.


#include "GameModes/CCWorldSettings.h"

#include "Engine/AssetManager.h"
#include "Macro/CCLogMacro.h"

FPrimaryAssetId ACCWorldSettings::GetDefaultGameplayExperience() const
{
	FPrimaryAssetId Result;
	if (!DefaultGameplayExperience.IsNull()) {
		Result = UAssetManager::Get().GetPrimaryAssetIdForPath(DefaultGameplayExperience.ToSoftObjectPath());

		if (!Result.IsValid()) {
			DEBUG_ERROR("%s.DefaultGameplayExperience is %s but that failed to resolve into an asset ID (you might need to add a path to the Asset Rules in your game feature plugin or project settings",
				*GetPathNameSafe(this), *DefaultGameplayExperience.ToString());
		}
	}
	return Result;
}
