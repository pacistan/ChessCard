#pragma once

#include "CoreMinimal.h"
#include "GameModes/FSM/CCBaseState.h"
#include "CCStateDrawingCards.generated.h"

class ACCPlayerPawn;


UCLASS()
class CHESSCARD_API UCCStateDrawingCards : public UCCBaseState
{
	GENERATED_BODY()
	/* ------------------------------------------ MEMBERS -------------------------------------------*/
protected:
	UPROPERTY()
	TArray<TObjectPtr<ACCPlayerPawn>> Players;

	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
	
	UFUNCTION()
	void OnPlayerEndDraw(ACCPlayerPawn* Player);
	
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
	virtual void Initialization() override;
	virtual void OnEnterState() override;
	virtual void OnExitState() override;

};
