// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "CCPlayerStart.generated.h"

UCLASS(HideDropdown)
class CHESSCARD_API ACCPlayerStart : public APlayerStart
{
	GENERATED_BODY()
	/* ------------------------------------------ MEMBERS -------------------------------------------*/
protected:
	/** The controller that claimed this PlayerStart */
	UPROPERTY(Transient)
	TObjectPtr<AController> ClaimingController = nullptr;
	
	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
public:
	/** Did this player start get claimed by a controller already? */
	bool IsClaimed() const;
	
	/** If this PlayerStart was not claimed, claim it for ClaimingController */
	bool TryClaim(AController* OccupyingController);
	
protected:
	/* ------------------------------------------ OVERRIDE -------------------------------------------*/
};
