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
	const FCardData* CardData = DeckCards[0].DataTable ? DeckCards[0].GetRow<FCardData>(TEXT("Data of Card")) : nullptr;

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

void UCCDeckComponent::BeginPlay()
{
	Super::BeginPlay();
}
