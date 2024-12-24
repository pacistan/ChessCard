#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Macro/CCGetSetMacro.h"
#include "CCGameState.generated.h"

class ACCPlayerController;
class ACCGridManager;

UCLASS()
class CHESSCARD_API ACCGameState : public AGameStateBase
{
	GENERATED_BODY()
	/* ------------------------------------------ MEMBERS -------------------------------------------*/
protected:
	friend class ACCGameMode;
	
	UPROPERTY(Replicated)
	TObjectPtr<ACCGridManager> GridManager;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Replicated)
	TArray<ACCPlayerController*> PlayerControllers;
	
	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
public:
	ACCGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
	bool AddPlayerController(ACCPlayerController* PlayerController);
	
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
private:
	virtual void BeginPlay() override;
	
	/* ------------------------------------------ GETTER/SETTER -------------------------------------------*/
public:
	UFUNCTION(BlueprintGetter)
	ACCGridManager* GetGridManager()const {return GridManager;}

	UFUNCTION()
	int GetIndexOfPlayerController(ACCPlayerController* PlayerController) const {return PlayerControllers.Find(PlayerController);} 
};
