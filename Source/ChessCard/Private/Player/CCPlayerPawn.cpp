#include "Player/CCPlayerPawn.h"

#include "Blueprint/UserWidget.h"
#include "Camera/CameraComponent.h"
#include "Card/CCCard.h"
#include "Card/CCCardMovementComponent.h"
#include "Deck/CCDeckComponent.h"
#include "Grid/CCTile.h"
#include "Hand/CCHandComponent.h"
#include "Macro/CCLogMacro.h"
#include "UI/CCMainWidget.h"


ACCPlayerPawn::ACCPlayerPawn(const FObjectInitializer& ObjectInitializer): Super (ObjectInitializer)
{
	PrimaryActorTick.bCanEverTick = false;
	RootComponent = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(RootComponent);
	DeckComponent = CreateDefaultSubobject<UCCDeckComponent>(TEXT("Deck"));
	MovementDeckComponent = CreateDefaultSubobject<UCCDeckComponent>(TEXT("Movement Deck"));
	HandComponent = CreateDefaultSubobject<UCCHandComponent>(TEXT("Hand"));
	//FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	//FollowCamera->bUsePawnControlRotation = true;
	
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
	OnCardMovementEnd.AddDynamic(this, &ACCPlayerPawn::DrawCard);
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

void ACCPlayerPawn::OnGetMovementCardTrigger()
{
	if(CurrentSelectedCardIndex == -1)
	{
		DEBUG_ERROR("No Selected Card");
		return;
	}
	FOnCardMovementEnd CardMovementEnd;
	CardMovementEnd.AddDynamic(this, &ACCPlayerPawn::RemoveCardFromHand);
	CardMovementEnd.AddDynamic(this, &ACCPlayerPawn::DrawMovementCard);
	HandComponent->SendSelectedCardToMovementDeck(CurrentSelectedCardIndex, CardMovementEnd, MovementDeckComponent->DeckPosition);
}

void ACCPlayerPawn::DrawMovementCard()
{
	FOnCardMovementEnd OnCardMovementEnd;
	ACCCard* Card = MovementDeckComponent->CreateCard();
	HandComponent->DrawCard(Card, OnCardMovementEnd);
}

void ACCPlayerPawn::RemoveCardFromHand()
{
	HandComponent->RemoveCardFromHand(CurrentSelectedCardIndex);
	SetCurrentSelectedCardIndex(-1);
}

void ACCPlayerPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);
	if(MainWidgetClass)
	{
		MainWidget = Cast<UCCMainWidget>(CreateWidget(GetWorld(), MainWidgetClass));
		if(MainWidget)
		{
			MainWidget->AddToViewport();
		}
	}
}

void ACCPlayerPawn::SetCurrentSelectedCardIndex(int32 InSelectedCardIndex)
{
	CurrentSelectedCardIndex = InSelectedCardIndex;
	MainWidget->OnSelectCardChange(CurrentSelectedCardIndex == -1? false : true);
}

void ACCPlayerPawn::OnAllCardDrawServer_Implementation()
{
	EndDrawDelegate.ExecuteIfBound(this);
}
