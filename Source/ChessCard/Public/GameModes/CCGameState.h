#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Macro/CCGetSetMacro.h"
#include "CCGameState.generated.h"

class UCCExperienceManagerComponent;
class ACCGridManager;
class ACCPlayerStart;
class ACCPlayerState;
class UCCPawnData;

UCLASS()
class CHESSCARD_API ACCGameState : public AGameStateBase
{
	GENERATED_BODY()
	/* ------------------------------------------ MEMBERS -------------------------------------------*/

private:
	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<ACCPlayerStart>> CachedPlayerStarts;

	UPROPERTY()
	TObjectPtr<ACCGridManager> GridManager;
	
	UPROPERTY()
	TObjectPtr<UCCExperienceManagerComponent> ExperienceManagerComponent;
	
	friend class ACCGameMode;
	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
protected:
	APlayerStart* GetFirstRandomUnoccupiedPlayerStart(AController* Controller, const TArray<ACCPlayerStart*>& FoundStartPoints) const;

public:
	ACCGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

private:
	AActor* ChoosePlayerStart(AController* Player);
	void FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation);

#if WITH_EDITOR
	APlayerStart* FindPlayFromHereStart(AController* Player);
#endif
	
	void HandleOnActorSpawned(AActor* SpawnedActor);
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
	
private:
	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;
	
	/* ------------------------------------------ GETTER/SETTER -------------------------------------------*/
public:
	UFUNCTION(BlueprintGetter)
	ACCGridManager* GetGridManager()const {return GridManager;}
};
