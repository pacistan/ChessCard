#include "Player/CCPlayerPawn.h"

#include "Camera/CameraComponent.h"
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
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->bUsePawnControlRotation = false;
}

void ACCPlayerPawn::DrawCards_Implementation(int NumberOfCardsToDraw)
{
	NumberOfCardsToDrawThisRound = NumberOfCardsToDraw;
	NumberOfCardDrawnOnRoundStart = 0;
	FTimerHandle TimerHandle;
	GetWorldTimerManager().SetTimer(TimerHandle, [this]()
	{
		DrawCard();
	}, 1, false);
}

void ACCPlayerPawn::DrawCard()
{
	if(NumberOfCardsToDrawThisRound == NumberOfCardDrawnOnRoundStart)
	{
		OnAllCardDrawServer();
		return;
	}
	
	FOnCardMovementEnd OnCardMovementEnd;
	OnCardMovementEnd.BindDynamic(this, &ACCPlayerPawn::DrawCard);
	ACCCard* Card = DeckComponent->CreateCard();
	NumberOfCardDrawnOnRoundStart++;
	HandComponent->DrawCard(Card, OnCardMovementEnd);
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
