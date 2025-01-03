#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "CCDeckComponent.generated.h"

UENUM()
enum class EAddCardType :uint8{ Top, Bottom, Random };
class ACCCard;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), HideCategories(Variable, "Sockets", Tags, Component_Tick, Component_Replication, Activation, Cooking, Events, Asset_User_Data, Replication, Navigation))
class CHESSCARD_API UCCDeckComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCCDeckComponent();

	/* ------------------------------------------ MEMBERS -------------------------------------------*/
	UPROPERTY(EditAnywhere)
	TSubclassOf<ACCCard> CardPrefab;
	
	UPROPERTY(EditAnywhere)
	TArray<FDataTableRowHandle> DeckCards;

	UPROPERTY(EditAnywhere)
	FVector DeckPosition;
	
	/* -----------------------------------------FUNCTIONS -------------------------------------------*/
	UFUNCTION()
	ACCCard* CreateCard(bool ConsumeCard = true);

	UFUNCTION()
	void GenerateDeck(TArray<FDataTableRowHandle>& NewCards);
	
	UFUNCTION()
	void AddCardToDeck(FDataTableRowHandle CardRowHandle, EAddCardType AddType = EAddCardType::Random);

	UFUNCTION()
	void Shuffle();
	
	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
protected:
	virtual void BeginPlay() override;
};
