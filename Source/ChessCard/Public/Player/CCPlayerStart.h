// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "CCPlayerStart.generated.h"

UENUM()
enum class ETeam : uint8
{
	None   UMETA(DisplayName = "None"),
	One    UMETA(DisplayName = "Player1"),
	Two    UMETA(DisplayName = "Player2"),
	Three  UMETA(DisplayName = "Player3"),
	Four   UMETA(DisplayName = "Player4"),
};

UCLASS()
class CHESSCARD_API ACCPlayerStart : public APlayerStart
{
	GENERATED_BODY()
	/* ------------------------------------------ MEMBERS -------------------------------------------*/
protected:
	/** The controller that claimed this PlayerStart */
	UPROPERTY(Transient, VisibleInstanceOnly)
	TObjectPtr<AController> ClaimingController = nullptr;

	/** The Team Who will be Assignated to the Controller that Claimed this PlayerStart */
	UPROPERTY(EditAnywhere)
	ETeam Team = ETeam::None;
	
	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
public:
	/** Did this player start get claimed by a controller already? */
	bool IsClaimed() const;
	
	/** If this PlayerStart was not claimed, claim it for ClaimingController */
	bool TryClaim(AController* OccupyingController);

	/** Get the controller that claimed this PlayerStart */
	AController* GetClaimingController() const { return ClaimingController; }
	
protected:
	/* ------------------------------------------ OVERRIDE -------------------------------------------*/
};
