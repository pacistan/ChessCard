#include "Player/CCPlayerPawn.h"

#include "Card/CCCard.h"
#include "Card/CCCardMovementComponent.h"
#include "Deck/CCDeckComponent.h"
#include "Grid/CCTile.h"
#include "Hand/CCHandComponent.h"


ACCPlayerPawn::ACCPlayerPawn()
{
	PrimaryActorTick.bCanEverTick = false;
	DeckComponent = CreateDefaultSubobject<UCCDeckComponent>(TEXT("Deck"));
	HandComponent = CreateDefaultSubobject<UCCHandComponent>(TEXT("Hand"));
}

void ACCPlayerPawn::DrawCards(int NumberOfCardsToDraw)
{
	NumberOfCardsToDrawThisRound = NumberOfCardsToDraw;
	NumberOfCardDrawnOnRoundStart = 0;
	DrawCard();
}

void ACCPlayerPawn::DrawCard()
{
	if(NumberOfCardsToDrawThisRound == NumberOfCardDrawnOnRoundStart)
	{
		OnAllCardDrawServer();
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("DrawCard"));
	auto Card = DeckComponent->CreateCard(FVector(0));
	HandComponent->Cards.Add(Card);
	NumberOfCardDrawnOnRoundStart++;

	UE_LOG(LogTemp, Warning, TEXT("Card Num = %i"), HandComponent->GetCardNum());
	return;
	FOnCardMovementEnd OnCardMovementEnd;
	OnCardMovementEnd.BindDynamic(this, &ACCPlayerPawn::DrawCard);
	int CardNum = HandComponent->GetCardNum();
	HandComponent->Cards[CardNum - 1]->CardMovement->StartMovement(CardNum, CardNum, OnCardMovementEnd);
}

void ACCPlayerPawn::PlaySelectedCard(ACCTile* Tiles)
{
	auto& Card = HandComponent->Cards[CurrentSelectedCardIndex];
	Card->UnSelect(this);
	Card->Play(this);
	PlayedCardsIndex.Add(Card->CardIndex);
}

void ACCPlayerPawn::OnAllCardDrawServer_Implementation()
{
	EndDrawDelegate.ExecuteIfBound(this);
}
