#include "Player/CCPlayerPawn.h"
#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Card/CCCard.h"
#include "Card/CCCardMovementComponent.h"
#include "Card/FCardData.h"
#include "Deck/CCDeckComponent.h"
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
	HandComponent = CreateDefaultSubobject<UCCHandComponent>(TEXT("Hand"));
}

void ACCPlayerPawn::RPC_DrawCards_Implementation(int NumberOfCardsToDraw)
{
	NumberOfCardsToDrawThisRound = NumberOfCardsToDraw;
	NumberOfCardDrawnOnRoundStart = 0;
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, FTimerDelegate::CreateLambda([this](){ DrawCard(); }), 1, false);
}

void ACCPlayerPawn::RPC_SendQueueOfAction_Implementation()
{
	SRV_SendQueueOfAction(QueueOfPlayerActions);
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
	TArray<AActor*> Units;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ACCTileUnit::StaticClass(), Units);
	for(auto Unit : Units) Cast<ACCTileUnit>(Unit)->IsMoved = false;
}

void ACCPlayerPawn::DrawCard()
{
	
	if(NumberOfCardsToDrawThisRound == NumberOfCardDrawnOnRoundStart)
	{
		SRV_OnAllCardDrawServer();
		return;
	}
	
	FOnCardMovementEnd OnCardMovementEnd;
	OnCardMovementEnd.AddDynamic(this, &ACCPlayerPawn::DrawCard);

	
	ACCCard* Card = DeckComponent->CreateCard();
	if(Card->CardRowHandle.GetRow<FCardData>(TEXT(""))->CardName == TEXT("Gold"))
	{
		OnCardMovementEnd.AddDynamic(this, &ACCPlayerPawn::RemoveLastDrawnCardFromHand);
	}
	Card->Initialize();
	Card->SetOwningPawn(this);
	NumberOfCardDrawnOnRoundStart++;
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
	if(CurrentSelectedCardIndex == -1)
	{
		DEBUG_ERROR("No Selected Card");
		return;
	}
	FOnCardMovementEnd CardMovementEnd;
	CardMovementEnd.AddDynamic(this, &ACCPlayerPawn::RemoveSelectedCardFromHand);
	CardMovementEnd.AddDynamic(this, &ACCPlayerPawn::DrawMovementCard);
	HandComponent->SendSelectedCardToMovementDeck(CurrentSelectedCardIndex, CardMovementEnd, MovementDeckComponent->DeckPosition);
}

void ACCPlayerPawn::DrawMovementCard()
{
	FOnCardMovementEnd OnCardMovementEnd;
	ACCCard* Card = MovementDeckComponent->CreateCard();
	Card->Initialize();
	Card->SetOwningPawn(this);
	HandComponent->DrawCard(Card, OnCardMovementEnd);
}

void ACCPlayerPawn::RemoveSelectedCardFromHand()
{
	HandComponent->RemoveCardFromHand(CurrentSelectedCardIndex);
	SetCurrentSelectedCardIndex(-1);
}

void ACCPlayerPawn::RemoveLastDrawnCardFromHand()
{
	HandComponent->RemoveCardFromHand(HandComponent->Cards.Num() - 1);
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
		if(ACCTileUnit* Unit = Cast<ACCTileUnit>(RelatedActors[i]))
			Unit->DestroyPiece();
		else
			RelatedActors[i]->Destroy();
	}
	HandComponent->RemoveCardFromHand(QueueOfLocalActionElements[0].Card->CardIndex);
	QueueOfLocalActionElements.RemoveAt(0);
}

void ACCPlayerPawn::ForceEndTurn_Implementation()
{
	// TODO : Cancel Action if the player is in the middle of an action
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
	//OnSelectedCardChange.Broadcast(CurrentSelectedCardIndex);
}

void ACCPlayerPawn::SetSelectedUnit(ACCTileUnit* Unit)
{
	if(IsValid(SelectedUnit))
	{
		SelectedUnit->UnSelect();
	}
	SelectedUnit = Unit;
}

void ACCPlayerPawn::SRV_OnAllCardDrawServer_Implementation()
{
	EndDrawDelegate.ExecuteIfBound(this);
}
