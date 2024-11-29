// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Macro/CCGetSetMacro.h"
#include "CCGameState.generated.h"

class UCCExperienceManagerComponent;
class ACCPlayerStart;
class ACCPlayerState;
class UCCPawnData;

/**
 * 
 */
UCLASS()
class CHESSCARD_API ACCGameState : public AGameStateBase
{
	GENERATED_BODY()
	/* ------------------------------------------ MEMBERS -------------------------------------------*/

private:
	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<ACCPlayerStart>> CachedPlayerStarts;
	
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

public:
	virtual void Tick(float DeltaSeconds) override;

private:
	virtual void PostInitializeComponents() override;
};
