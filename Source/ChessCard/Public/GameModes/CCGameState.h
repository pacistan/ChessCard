﻿#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "Macro/CCGetSetMacro.h"
#include "CCGameState.generated.h"

class ACCGridManager;
class ACCPlayerStart;
class ACCPlayerState;
class UCCPawnData;

UCLASS(HideDropdown)
class CHESSCARD_API ACCGameState : public AGameStateBase
{
	GENERATED_BODY()
	/* ------------------------------------------ MEMBERS -------------------------------------------*/
public:
	/** The default pawn class to spawn for players */
	UPROPERTY(EditDefaultsOnly, Category=Gameplay)
	TSoftObjectPtr<const UCCPawnData> DefaultPawnData;
	
protected:
	UPROPERTY(Replicated)
	float ServerFPS;

private:
	UPROPERTY(Transient)
	TArray<TWeakObjectPtr<ACCPlayerStart>> CachedPlayerStarts;

	UPROPERTY()
	TObjectPtr<ACCGridManager> GridManager;
	
	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
protected:
	APlayerStart* GetFirstRandomUnoccupiedPlayerStart(AController* Controller, const TArray<ACCPlayerStart*>& FoundStartPoints) const;

public:
	ACCGameState(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	DECLARE_GETTER(ServerFPS, ServerFPS, float);

private:
	AActor* ChoosePlayerStart(AController* Player);
	void FinishRestartPlayer(AController* NewPlayer, const FRotator& StartRotation);
	friend class ACCGameMode;

#if WITH_EDITOR
	APlayerStart* FindPlayFromHereStart(AController* Player);
#endif
	
	void HandleOnActorSpawned(AActor* SpawnedActor);
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/

public:
	virtual void Tick(float DeltaSeconds) override;
	
private:
	virtual void PostInitializeComponents() override;

	virtual void BeginPlay() override;
	
	/* ------------------------------------------ GETTER/SETTER -------------------------------------------*/
public:
	UFUNCTION(BlueprintGetter)
	ACCGridManager* GetGridManager()const {return GridManager;}
};
