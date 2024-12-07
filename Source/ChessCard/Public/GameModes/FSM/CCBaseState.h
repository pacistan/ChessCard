#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "CCBaseState.generated.h"
	
UCLASS(Blueprintable, BlueprintType)
class CHESSCARD_API UCCBaseState : public UObject
{
	GENERATED_BODY()

	/* ------------------------------------------ MEMBERS -------------------------------------------*/
protected:
	TObjectPtr<ACCGameMode> GameMode; 
	
	/* ------------------------------------------ FUNCTIONS -----------------------------------------*/
public:
	UFUNCTION()
	virtual void Initialization(ACCGameMode* InGameMode);
	
	UFUNCTION()
	virtual void OnEnterState();

	UFUNCTION()
	virtual void OnStateTick(float DeltaTime);
	
	UFUNCTION()
	virtual void OnExitState();
	
	/* ------------------------------------------ OVERRIDE -----------------------------------------*/
	/* ------------------------------------------ STATIC -----------------------------------------*/
public:
	UFUNCTION()
	static UCCBaseState* MakeStateWithClass(TSubclassOf<UCCBaseState> NewStateClass, UObject* Outer);
};
