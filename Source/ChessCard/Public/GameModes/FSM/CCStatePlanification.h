#pragma once

#include "CoreMinimal.h"
#include "GameModes/FSM/CCBaseState.h"
#include "Player/CCPlayerState.h"
#include "CCStatePlanification.generated.h"


class ACCPlayerState;

UCLASS()
class CHESSCARD_API UCCStatePlanification : public UCCBaseState
{
	GENERATED_BODY()
protected:
	/* ------------------------------------------ PROPERTIES -------------------------------------------*/
	UPROPERTY()
	float CurrentTime;

	UPROPERTY()
	TArray<ACCPlayerState*> ReadyPlayers;
	
	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
	UFUNCTION()
	void PlayerEndTurnCallback(ACCPlayerState* Player, bool IsEndTurn);
	
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
public:
	virtual void Initialization() override;
	virtual void OnEnterState() override;
	virtual void OnStateTick(float DeltaTime) override;
	virtual void OnExitState() override;

};
