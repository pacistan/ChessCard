#include "Player/CCPlayerPawn.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Card/CCCard.h"
#include "Card/CCCardMovementComponent.h"
#include "Card/FCardData.h"
#include "Deck/CCDeckComponent.h"
#include "GameModes/CCEffectManagerComponent.h"
#include "GameModes/CCGameMode.h"
#include "GameModes/CCGameState.h"
#include "Grid/CCTile.h"
#include "Hand/CCHandComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Macro/CCLogMacro.h"
#include "Player/CCPlayerState.h"
#include "TileActor/CCPieceBase.h"
#include "TileActor/CCTileUnit.h"
#include "UI/CCMainWidget.h"


ACCPlayerPawn::ACCPlayerPawn(const FObjectInitializer& ObjectInitializer): Super (ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(RootComponent);
	DeckComponent = CreateDefaultSubobject<UCCDeckComponent>(TEXT("Deck"));
	MovementDeckComponent = CreateDefaultSubobject<UCCDeckComponent>(TEXT("Movement Deck"));
	EmbrasementDeckComponent = CreateDefaultSubobject<UCCDeckComponent>(TEXT("Embrasement Deck"));
	DiscardPileComponent = CreateDefaultSubobject<UCCDeckComponent>(TEXT("Discard Pile"));
	HandComponent = CreateDefaultSubobject<UCCHandComponent>(TEXT("Hand"));
}

void ACCPlayerPawn::RPC_DrawCards_Implementation(int NumberOfCardsToDraw)
{
	EmbrasementDeckComponent->CardPrefab = DeckComponent->CardPrefab;
	NumberOfCardsToDrawThisRound = IsFirstRound ? NumberOfCardsToDrawFirstRound : NumberOfCardsToDraw;
	if(IsFirstRound)
	{
		DeckComponent->Shuffle();
	}
	IsFirstRound = false;
	NumberOfCardDrawnOnRoundStart = 0;
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this](){ DrawCard(); }), 1, false);
}

void ACCPlayerPawn::RPC_SendQueueOfAction_Implementation()
{
	SRV_SendQueueOfAction(QueueOfPlayerActions);

	GetWorld()->GetGameState<ACCGameState>()->GetGridManager()->UnhighlightTiles();
	
	if(CurrentSelectedCardIndex != -1)
	{
		HandComponent->Cards[CurrentSelectedCardIndex]->UnSelect(this);
	}

	for(auto Card : HandComponent->Cards)
	{
		if(Card->IsFleeting && Card->CurrentCardState != ECardState::Played)
		{
			RemoveCardWithIndex(Card->CardIndex);
		}
	}
}

void ACCPlayerPawn::SRV_SendQueueOfAction_Implementation(const TArray<FPlayerActionData>& ActionData)
{
	if (ACCGameMode* CCGameMode = GetWorld()->GetAuthGameMode<ACCGameMode>())
	{
		CCGameMode->AddPlayerAction(GetPlayerState<ACCPlayerState>(), ActionData);
	}
}

void ACCPlayerPawn::RPC_ClearActions_Implementation()
{
	QueueOfPlayerActions.Empty();
	OnActionQueueClear.Broadcast();
}

void ACCPlayerPawn::DrawCard()
{
	if(NumberOfCardsToDrawThisRound == NumberOfCardDrawnOnRoundStart || HandComponent->GetCardNum() == MaxNumberOfCardsInHand)
	{
		//Draw Embrasement Card
		ACCGameState* GameState = GetWorld()->GetGameState<ACCGameState>();
		auto& TileUnitCoordArray = GameState->GetGridManager()->MappedGrid[ETileType::Unit];
		for(auto Coordinates : TileUnitCoordArray)
		{
			ACCTile* Tile = GameState->GetGridManager()->GetTile(Coordinates);
			ACCPieceBase* Piece = Tile->GetPieces()[0];
			if(IsValid(Piece))
			{
				if(Piece->Team == GetPlayerState<ACCPlayerState>()->GetTeam())
				{
					if(Piece->CardDataRowHandle.GetRow<FCardData>("")->EffectType == EEffectType::Embrasement)
					{
						ACCCard* Card = EmbrasementDeckComponent->CreateCard(false);
						Card->Initialize();
						Card->SetOwningPawn(this);
						Card->IsCore = false;
						Card->IsFleeting = true;
						HandComponent->DrawCard(Card, FOnCardMovementEnd());
					}
				}
			}
			else
			{
				DEBUG_LOG("");
			}
		}
		SRV_OnAllCardDrawServer();
		return;
	}
	
	NumberOfCardDrawnOnRoundStart++;
	FOnCardMovementEnd OnCardMovementEnd;
	OnCardMovementEnd.AddDynamic(this, &ACCPlayerPawn::DrawCard);

	//Refill Deck
	if(DeckComponent->DeckCards.IsEmpty())
	{
		DeckComponent->GenerateDeck(DiscardPileComponent->DeckCards);
	}
	//DiscardPile Is Empty Stop Drawing
	if(DeckComponent->DeckCards.IsEmpty())
	{
		OnCardMovementEnd.Broadcast();
		DEBUG_ERROR("Deck and Discard Pile are empty but you are trying to draw");
		return;
	}
	
	ACCCard* Card = DeckComponent->CreateCard();

	Card->Initialize();
	Card->SetOwningPawn(this);
	if(Card->CardRowHandle.GetRow<FCardData>("")->EffectType == EEffectType::Gold)
	{
		Card->IsCore = false;
		Card->IsFleeting = true;
	}
	HandComponent->DrawCard(Card, OnCardMovementEnd);
}

void ACCPlayerPawn::PlaySelectedCard(ACCTile* Tile)
{
	auto& Card = HandComponent->Cards[CurrentSelectedCardIndex];
	Card->UnSelect(this);
	Card->Play(this);
	PlayedCardsIndex.Add(Card->CardIndex);
}

void ACCPlayerPawn::OnGetMovementCardTrigger()
{
	if(CurrentSelectedCardIndex == -1 || !HandComponent->Cards[CurrentSelectedCardIndex]->IsCore)
	{
		return;
	}
	GetWorld()->GetGameState<ACCGameState>()->GetGridManager()->UnhighlightTiles();
	FOnCardMovementEnd CardMovementEnd;
	CardMovementEnd.AddDynamic(this, &ACCPlayerPawn::RemoveSelectedCardFromHand);
	CardMovementEnd.AddDynamic(this, &ACCPlayerPawn::DrawMovementCard);
	HandComponent->SendSelectedCardToMovementDeck(CurrentSelectedCardIndex, CardMovementEnd, MovementDeckComponent->DeckPosition);
}

void ACCPlayerPawn::DrawMovementCard()
{
	FOnCardMovementEnd OnCardMovementEnd;
	ACCCard* Card = MovementDeckComponent->CreateCard(false);
	Card->Initialize();
	Card->SetOwningPawn(this);
	Card->IsCore = false;
	HandComponent->DrawCard(Card, OnCardMovementEnd);
}

void ACCPlayerPawn::RemoveSelectedCardFromHand()
{
	auto Handle = HandComponent->RemoveCardFromHand(CurrentSelectedCardIndex);
	SetCurrentSelectedCardIndex(-1);
	DiscardPileComponent->AddCardToDeck(Handle);
}

void ACCPlayerPawn::RemoveCardWithIndex(int TargetIndex)
{
	HandComponent->RemoveCardFromHand(TargetIndex);
}

void ACCPlayerPawn::AddPlayerAction(FPlayerActionData Action)
{
	QueueOfPlayerActions.Add(Action);
	OnActionQueueAdd.Broadcast(Action);
}

void ACCPlayerPawn::AddPlayerActionClientElement(TArray<AActor*>& Actors, ACCCard* Card)
{
	FActionLocalElements ActionLocalElements;
	ActionLocalElements.Card = Card;
	ActionLocalElements.RelatedActors = Actors;
	QueueOfLocalActionElements.Add(ActionLocalElements);
}

void ACCPlayerPawn::RPC_AddPlayerActionClientElement_Implementation(const TArray<AActor*>& Actors)
{
	FActionLocalElements ActionLocalElements;
	ActionLocalElements.RelatedActors = Actors;
	QueueOfLocalActionElements.Add(ActionLocalElements);
}
 
void ACCPlayerPawn::RemoveLastPlayerAction()
{
	QueueOfPlayerActions.Pop();
	OnActionQueueRemove.Broadcast();
}

void ACCPlayerPawn::RPC_RemoveFirstActionClientElements_Implementation()
{
	if(QueueOfLocalActionElements.IsEmpty())
	{
		return;
	}

	TArray<TObjectPtr<AActor>> RelatedActors = QueueOfLocalActionElements[0].RelatedActors;
	for(int i = RelatedActors.Num() - 1; i >= 0; i--)
	{
		if(ACCPieceBase* Unit = Cast<ACCPieceBase>(RelatedActors[i]))
			Unit->DestroyPiece();
		else
			RelatedActors[i]->Destroy();
	}

	if(IsValid(QueueOfLocalActionElements[0].Card))
	{
		auto Handle = HandComponent->RemoveCardFromHand(QueueOfLocalActionElements[0].Card->CardIndex);
		if(QueueOfLocalActionElements[0].Card->IsCore)
		{
			DiscardPileComponent->AddCardToDeck(Handle);
		}
	}
	QueueOfLocalActionElements.RemoveAt(0);
}

void ACCPlayerPawn::ForceEndTurn_Implementation()
{
	// TODO : Cancel Action if the player is in the middle of an action
	GetWorld()->GetGameState<ACCGameState>()->GetGridManager()->UnhighlightTiles();
	if (ACCPlayerState* CCPlayerState =  GetPlayerState<ACCPlayerState>()) {
		CCPlayerState->RPC_SetEndTurn(true);
	}
}

void ACCPlayerPawn::ClearPlayerAction_Implementation()
{
	QueueOfPlayerActions.Empty();
}

void ACCPlayerPawn::AddPlayerHud_Implementation()
{
	DEBUG_LOG("TEST : %s for %s", IsPlayerControlled() ? TEXT("True") :TEXT("False"), *GetName());
	if(PLayerHudClass) {
		PlayerHud = CreateWidget(GetWorld(), PLayerHudClass);
		if(PlayerHud) {
			PlayerHud->AddToPlayerScreen();
		}
	}
}

void ACCPlayerPawn::RPC_AddCardToDeck_Implementation(FDataTableRowHandle CardRowHandle, EAddCardType InAddCardType)
{
	DeckComponent->AddCardToDeck(CardRowHandle);
}

void ACCPlayerPawn::UnPossessed()
{
	Super::UnPossessed();
	if (IsValid(PlayerHud)) {
		PlayerHud->RemoveFromParent();
		PlayerHud = nullptr;
	}
}

void ACCPlayerPawn::SetCurrentSelectedCardIndex(int32 InSelectedCardIndex)
{
	if(CurrentSelectedCardIndex != InSelectedCardIndex && IsValid(SelectedUnit))
	{
		SelectedUnit->UnSelect();
		SelectedUnit = nullptr;
	}
	CurrentSelectedCardIndex = InSelectedCardIndex;
	OnSelectedCardChange.Broadcast(CurrentSelectedCardIndex);
}

void ACCPlayerPawn::SetSelectedUnit(ACCPieceBase* Unit)
{
	if(IsValid(SelectedUnit))
	{
		SelectedUnit->UnSelect();
	}
	SelectedUnit = Unit;
}

ACCPieceBase* ACCPlayerPawn::GetSelectedUnit()
{
	return SelectedUnit;
}

void ACCPlayerPawn::SRV_OnAllCardDrawServer_Implementation()
{
	EndDrawDelegate.ExecuteIfBound(this);
}
