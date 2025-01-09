// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerStart.h"
#include "CCPlayerStart.generated.h"

//DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPlayerStartClaimed, AController*, ClaimingController);

UENUM(BlueprintType)
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
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	ETeam Team = ETeam::None;

public:
	/** Event that is broadcasted when this PlayerStart is claimed */
	//UPROPERTY(BlueprintAssignable)
	//FOnPlayerStartClaimed OnPlayerStartClaimed;
	
	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
public:
	/** Did this player start get claimed by a controller already? */
	bool IsClaimed() const;
	
	/** If this PlayerStart was not claimed, claim it for ClaimingController */
	bool TryClaim(AController* OccupyingController);

	UFUNCTION(BlueprintImplementableEvent)
	void OnPlayerStartClaimed(AController* InController);

	/** Get the controller that claimed this PlayerStart */
	AController* GetClaimingController() const { return ClaimingController; }
	
protected:
	/* ------------------------------------------ OVERRIDE -------------------------------------------*/
};
