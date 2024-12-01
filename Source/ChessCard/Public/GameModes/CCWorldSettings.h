// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/WorldSettings.h"
#include "CCWorldSettings.generated.h"

class UCCExperienceDefinition;
/**
 * 
 */
UCLASS()
class CHESSCARD_API ACCWorldSettings : public AWorldSettings
{
	GENERATED_BODY()
	/* ---------------------------------- MEMBERS ------------------------------------------ */
protected:
	UPROPERTY(EditDefaultsOnly, Category = GameMode)
	TSoftClassPtr<UCCExperienceDefinition> DefaultGameplayExperience;
	
	/* ---------------------------------- FUNCTIONS ------------------------------------------ */
	
public:
	FPrimaryAssetId GetDefaultGameplayExperience() const;
};
