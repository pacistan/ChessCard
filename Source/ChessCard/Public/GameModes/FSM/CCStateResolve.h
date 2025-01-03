#pragma once

#include "CoreMinimal.h"
#include "CCBaseState.h"
#include "Player/CCPlayerPawn.h"
#include "Player/CCPlayerState.h"
#include "CCStateResolve.generated.h"

DECLARE_MULTICAST_DELEGATE(FKillDeathDelegate);


UCLASS()
class CHESSCARD_API UCCStateResolve : public UCCBaseState
{
	GENERATED_BODY()

	UPROPERTY()
	int NumberOfActionResolved;
		
	UPROPERTY()
	bool AreAllPlayerQueuesSent = false;

	TMap<ACCTile*, TArray<ACCPieceBase*>> KillerTiles;

	TMap<ACCPieceBase*, FPlayerActionData> LastActions;
	
	UPROPERTY()
	TSet<ACCTile*> SlaughterTiles;
	
	UFUNCTION()	
	void OnAllPlayerQueuesSent();

	UFUNCTION()
	void OnActionResolved(ACCPlayerState* PlayerState, FPlayerActionData LastAction, ACCPieceBase* LastPiece);

	UFUNCTION()
	void ApplyActionEffects(ACCPlayerState* PlayerState, const FPlayerActionData& LastAction, ACCPieceBase* LastPiece, bool IsMovementAction);

	UFUNCTION()
	void BloodSlaughterAndDeath();
	void AddKillLambda(FKillDeathDelegate& Delegate, ACCTile* Tile, ACCPieceBase* Piece);
	void AddDeathLambda(FKillDeathDelegate& Delegate, ACCTile* Tile, ACCPieceBase* Piece);

	UFUNCTION()
	void StartNewAction();
	
public:
	virtual void OnEnterState() override;

	virtual void OnStateTick(float DeltaTime) override;

	virtual void OnExitState() override;
	
	FTimerHandle ActionTimerHandle;
	
};
