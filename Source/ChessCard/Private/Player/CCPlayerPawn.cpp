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
#include "Player/CCPlayerController.h"
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
	GetController<ACCPlayerController>()->CurrentSelectedElement.SetObject(nullptr);
	GetController<ACCPlayerController>()->CurrentSelectedElement.SetInterface(nullptr);
	//QueueOfLocalActionElements.Empty();
}

void ACCPlayerPawn::RPC_SendQueueOfAction_Implementation()
{
	if(HasAuthority())
	{
		DEBUG_ERROR("");
	}
	SRV_SendQueueOfAction(QueueOfPlayerActions);

	GetWorld()->GetGameState<ACCGameState>()->GetGridManager()->UnhighlightTiles();
	
	if(IsValid(CurrentSelectedCardIndex))
	{
		CurrentSelectedCardIndex->UnSelect(this);
	}

	for(auto Card : HandComponent->Cards)
	{
		if(Card->IsFleeting && Card->CurrentCardState != ECardState::Played)
		{
			RemoveCardWithIndex(Card->CardIndex);
		}
	}
	for(auto& LocalAction : QueueOfLocalActionElements)
	{
		for(int i = LocalAction.RelatedActors.Num() - 1; i >= 0; i--)
		{
			if(ACCPieceBase* Piece = Cast<ACCPieceBase>(LocalAction.RelatedActors[i]))
			{
				if(Piece->InitilizationProperties.IsPreview)
				{
					Piece->DestroyPiece();
					LocalAction.RelatedActors.RemoveAt(i);
				}
				else
				{
					Piece->SetActorHiddenInGame(false);
					Piece->SetActorEnableCollision(true);
					DEBUG_ERROR("TRYING TO DESTROY NON PREVIEW ELEMENT");
				}
			}
		}
	}

	for(auto Piece : NotBlueprintHiddenPieces)
	{
		Piece->SetActorHiddenInGame(false);
		Piece->SetActorEnableCollision(true);
		if(Piece->GetIsPreview())
		{
			DEBUG_LOG("");
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
	if(!IsValid(CurrentSelectedCardIndex))
	{
		DEBUG_ERROR("TRY TO PLAY WHILE NO CARD SELECTED");
		return;
	}
	auto Card = CurrentSelectedCardIndex;
	Card->UnSelect(this);
	Card->Play(this);
	PlayedCards.Add(Card);
}

void ACCPlayerPawn::OnGetMovementCardTrigger()
{
	if(!IsValid(CurrentSelectedCardIndex) || !CurrentSelectedCardIndex->IsCore)
	{
		return;
	}
	GetWorld()->GetGameState<ACCGameState>()->GetGridManager()->UnhighlightTiles();
	FOnCardMovementEnd CardMovementEnd;
	DiscardCardIndex = GetCurrentSelectedCardIndex()->CardIndex;
	SetCurrentSelectedCardIndex(nullptr);
	CardMovementEnd.AddDynamic(this, &ACCPlayerPawn::RemoveSelectedCardFromHand);
	CardMovementEnd.AddDynamic(this, &ACCPlayerPawn::DrawMovementCard);
	HandComponent->DiscardCard(DiscardCardIndex, CardMovementEnd, MovementDeckComponent->DeckPosition);
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
	auto Handle = HandComponent->RemoveCardFromHand(DiscardCardIndex);
	//SetCurrentSelectedCardIndex(-1);
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
	if(!RelatedActors.IsEmpty())
	{
		for(int i = RelatedActors.Num() - 1; i >= 0; i--)
		{
			if(IsValid(RelatedActors[i]))
			{
				if(ACCPieceBase* Unit = Cast<ACCPieceBase>(RelatedActors[i]))
					Unit->DestroyPiece();
				else
					RelatedActors[i]->Destroy();
			}
		}
	}

	if(IsValid(QueueOfLocalActionElements[0].Card))
	{
		auto Handle = QueueOfLocalActionElements[0].Card->GetDataTableRow();
		FOnCardMovementEnd CardMovementEnd;
		CardMovementEnd.AddDynamic(this, &ACCPlayerPawn::RemoveSelectedCardFromHand);
		DiscardCardIndex = QueueOfLocalActionElements[0].Card->CardIndex;
		HandComponent->DiscardCard(QueueOfLocalActionElements[0].Card->CardIndex, CardMovementEnd, MovementDeckComponent->DeckPosition);
		if(QueueOfLocalActionElements[0].Card->IsCore)
		{
			DiscardPileComponent->AddCardToDeck(Handle);
		}
		PlayedCards.RemoveAt(0);
	}
	QueueOfLocalActionElements.RemoveAt(0);
}

void ACCPlayerPawn::ForceEndTurn_Implementation()
{
	// TODO : Cancel Action if the player is in the middle of an action
	GetWorld()->GetGameState<ACCGameState>()->GetGridManager()->UnhighlightTiles();
	BPE_OnTimerEnd();
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

void ACCPlayerPawn::SetCurrentSelectedCardIndex(ACCCard* InSelectedCard)
{
	if(CurrentSelectedCardIndex != InSelectedCard && IsValid(SelectedUnit))
	{
		SelectedUnit->UnSelect();
		SelectedUnit = nullptr;
	}
	CurrentSelectedCardIndex = InSelectedCard;
	int CardIndexTarget = IsValid(CurrentSelectedCardIndex) ? CurrentSelectedCardIndex->CardIndex : -1;
	OnSelectedCardChange.Broadcast(CardIndexTarget);
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

void ACCPlayerPawn::UndoAction()
{
	if(!PlayedCards.IsEmpty() && GetWorld()->GetGameState<ACCGameState>()->GetCurrentState() == EGameState::Plannification)
	{
		BPE_OnUndoAction();
		ACCCard* Card = PlayedCards.Last();
		ECardType CardType = Card->CardRowHandle.GetRow<FCardData>("")->CardType;
		Card->Unplay(this);
		if(!QueueOfPlayerActions.Last().MovementData.IsEmpty())
		{
			ACCTileUnit* Unit = Cast<ACCTileUnit>(GetWorld()->GetGameState<ACCGameState>()->GetGridManager()->GetTile(QueueOfPlayerActions.Last().TargetCoord)->GetPieces().Last());
			Unit->MovementData = FUnitMovementData();
			Unit->IsMoved = false;
			Unit->LinkedCard = nullptr;
			Unit->MovementDatas.Empty();
		}
		else if(CardType == ECardType::Unit)
		{
			ACCTile* Tile = GetWorld()->GetGameState<ACCGameState>()->GetGridManager()->GetTile(QueueOfPlayerActions.Last().TargetCoord);
			if(Tile->GetPieces().Num() > 1)
			{
				Tile->GetPieces()[FMath::Min(Tile->GetPieces().Num() - 2, 0)]->SetActorHiddenInGame(false);
				Tile->GetPieces()[FMath::Min(Tile->GetPieces().Num() - 2, 0)]->SetActorEnableCollision(true);
			}
		}

		for(AActor* VisualActor : QueueOfLocalActionElements.Last().RelatedActors)
		{
			if(ACCPieceBase* Piece = Cast<ACCPieceBase>(VisualActor))
			{
				Piece->DestroyPiece();
			}
			else
			{
				VisualActor->Destroy();
			}
		}
			
		PlayedCards.Pop();
		RemoveLastPlayerAction();
		if(Card == CurrentSelectedCardIndex)
		{
			CurrentSelectedCardIndex = nullptr;
			GetController<ACCPlayerController>()->CurrentSelectedElement = nullptr;
		}
		QueueOfLocalActionElements.Pop();
	}
}
