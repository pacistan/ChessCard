#pragma once

#include "CoreMinimal.h"
#include "GameModes/FSM/CCBaseState.h"
#include "CCStateDrawingCards.generated.h"

class ACCPlayerPawn;


UCLASS()
class CHESSCARD_API UCCStateDrawingCards : public UCCBaseState
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<TObjectPtr<ACCPlayerPawn>> Players;
	
	virtual void Initialization(ACCGameMode* InGameMode) override;
	virtual void OnEnterState() override;

	UFUNCTION()
	void OnPlayerEndDraw(ACCPlayerPawn* Player);
};
