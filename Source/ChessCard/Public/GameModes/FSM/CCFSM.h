#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CCFSM.generated.h"

class UCCBaseState;
class ACCGameMode;

UCLASS()
class CHESSCARD_API UCCFSM : public UObject
{
	GENERATED_BODY()

public:
	UCCFSM();
	
protected:
	UPROPERTY()
	TObjectPtr<UCCBaseState> CurrentState;

	UPROPERTY()
	TObjectPtr<ACCGameMode> Owner;
	
	/* ------------------------------------------ FUNCTIONS -----------------------------------------*/
protected:

public:
	UFUNCTION()
	void OnBeginPlay() ;

	UFUNCTION()
	void OnTick(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	bool ChangeStateWithClass(TSubclassOf<UCCBaseState> NewStateClass);

	TObjectPtr<UCCBaseState> GetCurrentState() const {return CurrentState;}
};
