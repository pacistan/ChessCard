#include "Deck/CCDeckComponent.h"
#include "Card/CCCard.h"
#include "Card/FCardData.h"
#include "Kismet/GameplayStatics.h"
#include "Macro/CCLogMacro.h"


UCCDeckComponent::UCCDeckComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

ACCCard* UCCDeckComponent::CreateCard()
{
	auto DeckCard = DeckCards[0];
	DeckCards.RemoveAt(0);
	const FCardData* CardData = DeckCard.DataTable ? DeckCards[0].GetRow<FCardData>(TEXT("Data of Card")) : nullptr;
	check(CardData);
	const APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	const FRotator CameraRotation = CameraManager->GetCameraRotation();
	const FVector TransformedDeckPosition = FRotationMatrix(CameraRotation).TransformVector(DeckPosition);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.bNoFail = true;
	SpawnParams.Owner = GetOwner();
	auto Card =  GetWorld()->SpawnActor<ACCCard>(CardPrefab, TransformedDeckPosition, CameraRotation, SpawnParams);
	Card->AttachToActor(GetOwner(), FAttachmentTransformRules::KeepWorldTransform);
	Card->SetDataTableRow(DeckCards[0]);
	
	return Card;
}

void UCCDeckComponent::GenerateDeck(TArray<FDataTableRowHandle>& NewCards)
{
	while(!NewCards.IsEmpty())
	{
		int TransferIndex = FMath::RandRange(0, NewCards.Num() - 1);
		DeckCards.Add(NewCards[TransferIndex]);
		NewCards.RemoveAt(TransferIndex);
	}
}

void UCCDeckComponent::AddCardToDeck(FDataTableRowHandle CardRowHandle, EAddCardType AddType)
{
	int InsertIndex = 0;
	switch(AddType)
	{
	case EAddCardType::Top:
		break;
	case EAddCardType::Bottom:
		InsertIndex = DeckCards.Num();
		break;
	case EAddCardType::Random:
		InsertIndex = FMath::RandRange(0, DeckCards.Num());
		break;
	}
	DeckCards.Insert(CardRowHandle, InsertIndex);
}

void UCCDeckComponent::BeginPlay()
{
	Super::BeginPlay();
}
