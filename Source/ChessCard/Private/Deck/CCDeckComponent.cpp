#include "Deck/CCDeckComponent.h"
#include "Card/CCCard.h"
#include "Card/FCardData.h"
#include "Kismet/GameplayStatics.h"
#include "Macro/CCLogMacro.h"


UCCDeckComponent::UCCDeckComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

ACCCard* UCCDeckComponent::CreateCard(bool ConsumeCard)
{
	auto DeckCard = DeckCards[0];
	if(ConsumeCard)
	{
		DeckCards.RemoveAt(0);
	}
	const FCardData* CardData = DeckCard.DataTable ? DeckCard.GetRow<FCardData>(TEXT("Data of Card")) : nullptr;
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
	Card->SetDataTableRow(DeckCard);
	
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
		InsertIndex = FMath::RandRange(0, DeckCards.Num() - 1);
		break;
	}
	DeckCards.Insert(CardRowHandle, InsertIndex);
}

void UCCDeckComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UCCDeckComponent::Shuffle()
{
	const int32 LastIndex = DeckCards.Num() - 1;
	for (int32 i = 0; i <= LastIndex; ++i)
	{
		const int32 Index = FMath::RandRange(i, LastIndex);
		if (i != Index)
		{
			 DeckCards.Swap(i, Index);
		}
	}
}