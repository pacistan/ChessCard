#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CCBaseState.generated.h"

class ACCGameState;
class ACCGameMode;

UCLASS(Blueprintable, BlueprintType)
class CHESSCARD_API UCCBaseState : public UObject
{
	GENERATED_BODY()

	/* ------------------------------------------ MEMBERS -------------------------------------------*/
protected:
	UPROPERTY()
	TObjectPtr<ACCGameState> CCGameState;

	UPROPERTY()
	TObjectPtr<ACCGameMode> CCGameMode;
	
	/* ------------------------------------------ FUNCTIONS -----------------------------------------*/
public:
	UFUNCTION()
	virtual void Initialization();
	
	UFUNCTION()
	virtual void OnEnterState();

	UFUNCTION()
	virtual void OnStateTick(float DeltaTime);
	
	UFUNCTION()
	virtual void OnExitState();
	
private:
	/* ------------------------------------------ OVERRIDE -----------------------------------------*/

	/* ------------------------------------------ GETTERS / SETTERS -----------------------------------------*/

	/* ------------------------------------------ STATIC -----------------------------------------*/
public:
	UFUNCTION()
	static UCCBaseState* MakeStateWithClass(TSubclassOf<UCCBaseState> NewStateClass, UObject* Outer);
};
