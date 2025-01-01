#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "Macro/CCGetSetMacro.h"
#include "Macro/CCLogMacro.h"
#include "CCPlayerState.generated.h"

class UCCExperienceDefinition;
class ACCPlayerController;
class UCCPawnData;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnPlayerEndTurn, ACCPlayerState*, Player, bool, IsEndTurn);


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

public:
	FOnPlayerEndTurn EndTurnDelegate;

	
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

	UFUNCTION(BlueprintCallable, Client, Unreliable)
	void RPC_SetEndTurn(bool bInEndTurn); 

	UFUNCTION(BlueprintCallable, Server, Unreliable)
	void SRV_SetEndTurn(bool bInEndTurn);

	UFUNCTION(BlueprintCallable)
	void SetEndTurn(bool bEndTurn);
};
