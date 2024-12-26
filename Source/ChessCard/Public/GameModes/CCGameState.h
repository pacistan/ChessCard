#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Macro/CCGetSetMacro.h"
#include "CCGameState.generated.h"

class ACCPlayerState;
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

	/** List of all player controllers in the game, only True in Server */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
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

	UFUNCTION()
	ACCPlayerState* GetPlayerStateOfTeam(ETeam Team) const;
};
