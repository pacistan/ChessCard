﻿#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Macro/CCGetSetMacro.h"
#include "Macro/CCLogMacro.h"
#include "CCPlayerState.generated.h"

class UCCExperienceDefinition;
class ACCPlayerController;
class UCCPawnData;

UCLASS()
class CHESSCARD_API ACCPlayerState : public APlayerState
{
	GENERATED_BODY()
	/* ------------------------------------------ MEMBERS -------------------------------------------*/
protected:
	
	// The player's team
	UPROPERTY(VisibleAnywhere, Replicated)
	ETeam Team;

	UPROPERTY(VisibleAnywhere, Replicated)
	bool bHasEndedTurn = false;
	
	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
public:
	ACCPlayerState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
public:
	virtual void BeginPlay() override;
	
	
	/* ------------------------------------------ GETTERS / SETTERS ------------------------------------*/
public:
	UFUNCTION(BlueprintCallable)
	ETeam GetTeam() const { return Team; }
	
	UFUNCTION(BlueprintCallable)
	void SetTeam(ETeam InTeam)
	{
		Team = InTeam;
		DEBUG_LOG_CATEGORY(LogCard, "Team of Player : %s", *UEnum::GetValueAsString(Team));
	}
	
	UFUNCTION(BlueprintCallable)
	bool GetHasEndedTurn() const { return bHasEndedTurn; }

	UFUNCTION(BlueprintCallable)
	void SetEndTurn(bool bInEndTurn) { bHasEndedTurn = bInEndTurn; }
	
};
