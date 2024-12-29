
#pragma once

#include "CoreMinimal.h"
#include "Card/FCardData.h"
#include "GameFramework/Pawn.h"
#include "CCPlayerPawn.generated.h"


class ACCTileUnit;
class ACCPieceBase;
class ACCCard;
class UCCMainWidget;
class FOnCardMovementEnd;
class UCCHandComponent;
class UCCDeckComponent;
class UCameraComponent;
class ACCTile;

// Data structure to store the action of the player 
USTRUCT(BlueprintType)
struct FPlayerActionData
{
	GENERATED_BODY()

	/* Data of the card use for the action */
	UPROPERTY()
	FDataTableRowHandle CardData; 
	
	/* Coord in the grid of the target of the tile */
	UPROPERTY()
	FIntPoint TargetCoord;
	
	// Struct of mvt if the card is a movement card (Tarray<FPatternEndpoint>)
	// TODO : I don't have the good Struct Now 

	/* Id of the card in the hand of the player */
	UPROPERTY()
	uint32 CardID;

	FPlayerActionData()
	: CardData(FDataTableRowHandle())
	, TargetCoord(FIntPoint::ZeroValue)
	, CardID(0)
	{
	}
	
	FPlayerActionData(FDataTableRowHandle InCardData, FIntPoint InTargetCoord, uint32 InCardID)
	: CardData(InCardData)
	, TargetCoord(InTargetCoord)
	, CardID(InCardID)
	{
	}
};

DECLARE_DYNAMIC_DELEGATE_OneParam(FOnEndDrawDelegate, ACCPlayerPawn*, Player);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSelectedCardChange, int, SelectedCardIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnActionQueueAdd, FPlayerActionData, Action);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnActiomQueueUpdate);

UCLASS(/*HideCategories(Rendering, Collision, HLOD, Physics, Events, Level_Instance, Cooking, World_Partition, Data_Layers,  Actor_Tick)*/)
class CHESSCARD_API ACCPlayerPawn : public APawn
{
	GENERATED_BODY()

	ACCPlayerPawn(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());
	
	/* ------------------------------------------ MEMBERS -------------------------------------------*/
private:
	UPROPERTY(EditAnywhere, Category="", meta=(AllowPrivateAccess))
	TObjectPtr<UCCDeckComponent> DeckComponent;

	UPROPERTY(EditAnywhere, Category="", meta=(AllowPrivateAccess))
	TObjectPtr<UCCDeckComponent> MovementDeckComponent;

	UPROPERTY(EditAnywhere, Category="", meta=(AllowPrivateAccess))
	TObjectPtr<UCCHandComponent> HandComponent;
	
	//UPROPERTY(EditAnywhere, Category = "Camera", meta=(AllowPrivateAccess))
	//UCameraComponent* FollowCamera;
	
	UPROPERTY(VisibleAnywhere)
	int NumberOfCardDrawnOnRoundStart;

	UPROPERTY(VisibleAnywhere)
	int NumberOfCardsToDrawThisRound;

	UPROPERTY()
	TArray<int32> PlayedCardsIndex;

	// -1 = No card Selected
	UPROPERTY()
	int32 CurrentSelectedCardIndex;

	// The player hud class to add to the player when the game start
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> PLayerHudClass;

	// Keep a reference to the player hud for removing it when the player is unpossessed
	UPROPERTY()
	TObjectPtr<UUserWidget> PlayerHud;

	UPROPERTY()
	TObjectPtr<ACCTileUnit> SelectedUnit;

	// Queue of action The player Has Do during the turn
	UPROPERTY(VisibleInstanceOnly)
	TArray<FPlayerActionData> QueueOfPlayerActions;

public:
	FOnEndDrawDelegate EndDrawDelegate;

	/** Delegate to call when the selected card change, Needed for Ui */
	UPROPERTY(BlueprintAssignable)
	FOnSelectedCardChange OnSelectedCardChange;

	UPROPERTY(BlueprintAssignable)
	FOnActionQueueAdd OnActionQueueAdd;
	
	UPROPERTY(BlueprintAssignable)
	FOnActiomQueueUpdate OnActionQueueRemove;
	
	UPROPERTY(BlueprintAssignable)
	FOnActiomQueueUpdate OnActionQueueClear;
	
	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
public:
	UFUNCTION(BlueprintCallable, Client, Unreliable)
	void RPC_DrawCards(int NumberOfCardsToDraw);

	UFUNCTION(BlueprintCallable, Client, Unreliable)
	void RPC_SendQueueOfAction();

	UFUNCTION(BlueprintCallable)
	void DrawCard();

	UFUNCTION(BlueprintCallable)
	void PlaySelectedCard(ACCTile* Tiles);

	UFUNCTION(Server, Reliable)
	void SRV_OnAllCardDrawServer();

	UFUNCTION(BlueprintCallable)
	void OnGetMovementCardTrigger();

	UFUNCTION(BlueprintCallable)
	void DrawMovementCard();

	UFUNCTION()
	void RemoveCardFromHand();

	/** Add the Player HUD to the player, need to be call on Start of the game */
	UFUNCTION(Client, Unreliable)
	void AddPlayerHud();
	
	UFUNCTION(BlueprintCallable)
	void AddPlayerAction(FPlayerActionData Action);

	// Popn the last action of the player in The Owing Client
	UFUNCTION(BlueprintCallable)
	void RemoveLastPlayerAction();

	// Clear the Array of Action of the player in The Owing Client
	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ClearPlayerAction();

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ForceEndTurn();
	
private:
	UFUNCTION(Server, Unreliable)
	void SRV_SendQueueOfAction();
	
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
public:
	virtual void UnPossessed() override;
	
	/* ------------------------------------------ GETTERS/SETTERS -------------------------------------------*/
public:
	UFUNCTION(BlueprintGetter)
	int32 GetCurrentSelectedCardIndex()const {return CurrentSelectedCardIndex;}
	
	UFUNCTION(BlueprintSetter)
	void SetCurrentSelectedCardIndex(int32 InSelectedCardIndex);

	UFUNCTION(BlueprintGetter)
	UCCHandComponent* GetHandComponent()const {return HandComponent;}

	UFUNCTION(BlueprintGetter)
	TArray<int32> GetPlayedCardIndex()const {return PlayedCardsIndex;}

	UFUNCTION()
	void SetSelectedUnit(ACCTileUnit* Unit);
	
};
