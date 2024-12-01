#include "Deck/CCDeckComponent.h"
#include "Card/CCCard.h"
#include "Card/FCardData.h"
#include "Kismet/GameplayStatics.h"


UCCDeckComponent::UCCDeckComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

ACCCard* UCCDeckComponent::CreateCard()
{
	const FCardData* CardData = CardDataTable->FindRow<FCardData>(DeckCards[0],TEXT("No Card Found with name"));

	const APlayerCameraManager* CameraManager = UGameplayStatics::GetPlayerCameraManager(GetWorld(), 0);
	const FRotator CameraRotation = CameraManager->GetCameraRotation();
	const FVector TransformedDeckPosition = FRotationMatrix(CameraRotation).TransformVector(DeckPosition);
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	SpawnParams.bNoFail = true;
	SpawnParams.Owner = GetOwner();
	auto Card =  GetWorld()->SpawnActor<ACCCard>(CardPrefab, TransformedDeckPosition, CameraRotation, SpawnParams);
	Card->AttachToActor(GetOwner(), FAttachmentTransformRules::KeepRelativeTransform);
	return Card;
}

void UCCDeckComponent::BeginPlay()
{
	Super::BeginPlay();
}
