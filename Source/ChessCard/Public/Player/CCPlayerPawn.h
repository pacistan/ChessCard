#pragma once

#include "CoreMinimal.h"
//#include "Card/FCardData.h"
#include "Deck/CCDeckComponent.h"
#include "TileActor/PatternMapEndPoint.h"
#include "GameFramework/Pawn.h"
#include "CCPlayerPawn.generated.h"

enum class EEffectType : uint8;
struct FCardData;
class UPlayerWidget;
class ACCTileUnit;
class ACCPieceBase;
class ACCCard;
class UCCMainWidget;
class FOnCardMovementEnd;
class UCCHandComponent;
class UCCDeckComponent;
class UCameraComponent;
class ACCTile;
enum class EAddCardType : uint8;

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
	
	/* Id of the card in the hand of the player */
	UPROPERTY()
	FGuid CardID;

	UPROPERTY()
	FGuid UnitID;
	
	// Struct of mvt if the card is a movement card (Tarray<FPatternEndpoint>)
	UPROPERTY()
	TArray<FPatternMapEndPoint> MovementData;

	UPROPERTY()
	bool IsDivineAnger;
	
	FPlayerActionData()
	: CardData(FDataTableRowHandle())
	, TargetCoord(FIntPoint::ZeroValue)
	{
	}
	
	FPlayerActionData(FDataTableRowHandle InCardData, FIntPoint InTargetCoord, FGuid InCardID, FGuid InUnitGuid, TArray<FPatternMapEndPoint> InMovementData = TArray<FPatternMapEndPoint>(), bool InIsDivineAnger = false)
	: CardData(InCardData)
	, TargetCoord(InTargetCoord)
	, CardID(InCardID)
	, UnitID(InUnitGuid)
	, MovementData(InMovementData)
	, IsDivineAnger(InIsDivineAnger)
	{
	}
};

USTRUCT()
struct FActionLocalElements
{
	GENERATED_BODY()

	UPROPERTY()
	TObjectPtr<ACCCard> Card;

	UPROPERTY()
	TArray<TObjectPtr<AActor>> RelatedActors;
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
	TObjectPtr<UCCDeckComponent> EmbrasementDeckComponent;
	
	UPROPERTY(EditAnywhere, Category="", meta=(AllowPrivateAccess))
	TObjectPtr<UCCDeckComponent> DiscardPileComponent;

	UPROPERTY(EditAnywhere, Category="", meta=(AllowPrivateAccess))
	TObjectPtr<UCCHandComponent> HandComponent;
	
	UPROPERTY(VisibleAnywhere)
	int NumberOfCardDrawnOnRoundStart;

	UPROPERTY(VisibleAnywhere)
	int NumberOfCardsToDrawThisRound;

	UPROPERTY()
	TArray<int32> PlayedCardsIndex;

	// -1 = No card Selected
	UPROPERTY()
	int32 CurrentSelectedCardIndex;

	UPROPERTY()
	TObjectPtr<ACCPieceBase> SelectedUnit;

	// Queue of action The player Has Do during the turn
	UPROPERTY(VisibleInstanceOnly)
	TArray<FPlayerActionData> QueueOfPlayerActions;

	UPROPERTY()
	TArray<FActionLocalElements> QueueOfLocalActionElements;
	
public:
	// The player hud class to add to the player when the game start
	UPROPERTY(EditAnywhere)
	TSubclassOf<UUserWidget> PLayerHudClass;

	// Keep a reference to the player hud for removing it when the player is unpossessed
	UPROPERTY()
	TObjectPtr<UUserWidget> PlayerHud;

	UPROPERTY(EditAnywhere)
	int MaxNumberOfCardsInHand;

	UPROPERTY(EditAnywhere)
	int NumberOfCardsToDrawFirstRound = 5;

	UPROPERTY()
	bool IsFirstRound = true;

	UPROPERTY()
	int DiscardCardIndex;

	
public:
	UPROPERTY()
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
	void RPC_ClearActions();
	
	UFUNCTION(BlueprintCallable, Client, Unreliable)
	void RPC_SendQueueOfAction();

	UFUNCTION(Server, Reliable)
	void SRV_SendQueueOfAction(const TArray<FPlayerActionData>& ActionData);
	
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
	void RemoveSelectedCardFromHand();

	UFUNCTION()
	void RemoveCardWithIndex(int TargetIndex);

	/** Add the Player HUD to the player, need to be call on Start of the game */
	UFUNCTION(Client, Unreliable)
	void AddPlayerHud();
	
	UFUNCTION(BlueprintCallable)
	void AddPlayerAction(FPlayerActionData Action);

	UFUNCTION(BlueprintCallable)
	void AddPlayerActionClientElement(TArray<AActor*>& Actors, ACCCard* Card);

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void RPC_AddPlayerActionClientElement(const TArray<AActor*>& Actors);
	
	// Pop the last action of the player in The Owing Client. Is it useful?
	// We can Clear queue on planification Start
	UFUNCTION(BlueprintCallable)
	void RemoveLastPlayerAction();

	// Pop Elements on Client as the actions are played out during resolve phase.
	// Should be called at end of Movement
	UFUNCTION(BlueprintCallable, Client, Reliable)
	void RPC_RemoveFirstActionClientElements();
	
	// Clear the Array of Action of the player in The Owing Client
	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ClearPlayerAction();

	UFUNCTION(BlueprintCallable, Client, Reliable)
	void ForceEndTurn();

	UFUNCTION(Client, Reliable)
	void RPC_AddCardToDeck(FDataTableRowHandle CardRowHandle, EAddCardType InAddCardType = EAddCardType::Random);
	
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
	void SetSelectedUnit(ACCPieceBase* Unit);

	UFUNCTION()
	ACCPieceBase* GetSelectedUnit();

	UFUNCTION(BlueprintCallable)
	void UndoAction();

	UFUNCTION(BlueprintImplementableEvent)
	void BPE_OnTimerEnd();

	UFUNCTION(BlueprintImplementableEvent)
	void BPE_OnUndoAction();
};
