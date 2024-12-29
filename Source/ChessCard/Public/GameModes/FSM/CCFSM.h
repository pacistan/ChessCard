#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CCFSM.generated.h"

class ACCGameState;
class UCCBaseState;
class ACCGameMode;

UENUM(BlueprintType)
enum class EGameState : uint8
{
	None UMETA(DisplayName="None"),
	DrawingCards UMETA(DisplayName="DrawingCards"), 
	Plannification UMETA(DisplayName="Plannification"),
	Resolve UMETA(DisplayName="Resolve"),
	EndGame UMETA(DisplayName="EndGame")
};

UCLASS()
class CHESSCARD_API UCCFSM : public UObject
{
	GENERATED_BODY()
	/* ------------------------------------------ MEMBERS -------------------------------------------*/
protected:
	UPROPERTY()
	TObjectPtr<UCCBaseState> CurrentState;
	
	TMap<TSubclassOf<UCCBaseState>, EGameState> StateMap;
	
	/* ------------------------------------------ FUNCTIONS -----------------------------------------*/
public:
	UCCFSM();

	UFUNCTION()
	void OnTick(float DeltaTime);

	UFUNCTION(BlueprintCallable)
	bool ChangeStateWithClass(TSubclassOf<UCCBaseState> NewStateClass);

	TObjectPtr<UCCBaseState> GetCurrentState() const {return CurrentState;}
};
