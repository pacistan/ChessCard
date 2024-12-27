#pragma once

#include "CoreMinimal.h"
#include "GameModes/FSM/CCBaseState.h"
#include "CCStatePlanification.generated.h"

UCLASS()
class CHESSCARD_API UCCStatePlanification : public UCCBaseState
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	float CurrentTime;
	
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
public:
	virtual void Initialization() override;
	virtual void OnEnterState() override;
	virtual void OnStateTick(float DeltaTime) override;
	virtual void OnExitState() override;

};
