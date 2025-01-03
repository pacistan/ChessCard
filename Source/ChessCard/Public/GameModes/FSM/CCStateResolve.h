﻿#pragma once

#include "CoreMinimal.h"
#include "CCBaseState.h"
#include "Player/CCPlayerPawn.h"
#include "Player/CCPlayerState.h"
#include "CCStateResolve.generated.h"

UCLASS()
class CHESSCARD_API UCCStateResolve : public UCCBaseState
{
	GENERATED_BODY()

	UPROPERTY()
	int NumberOfActionResolved;
		
	UPROPERTY()
	bool AreAllPlayerQueuesSent = false;

	TMap<ACCTile*, TArray<ACCPieceBase*>> KillerTiles;

	UPROPERTY()
	TSet<ACCTile*> SlaughterTiles;
	
	UFUNCTION()	
	void OnAllPlayerQueuesSent();

	UFUNCTION()
	void OnActionResolved(ACCPlayerState* PlayerState, FPlayerActionData LastAction, ACCPieceBase* LastPiece);

	UFUNCTION()
	void ApplyActionEffects(ACCPlayerState* PlayerState, const FPlayerActionData& LastAction, ACCPieceBase* LastPiece, bool IsMovementAction);
	void BloodSlaughterAndDeath();

	UFUNCTION()
	void StartNewAction();
	
public:
	virtual void OnEnterState() override;

	virtual void OnStateTick(float DeltaTime) override;

	virtual void OnExitState() override;
	
	FTimerHandle ActionTimerHandle;
	
};
