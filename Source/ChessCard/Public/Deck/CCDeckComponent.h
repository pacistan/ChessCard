#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"

#include "CCDeckComponent.generated.h"


class ACCCard;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), HideCategories
	(Variable, "Sockets", Tags, Component_Tick, Component_Replication, Activation, Cooking, Events, Asset_User_Data, Replication, Navigation))
class CHESSCARD_API UCCDeckComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCCDeckComponent();

	/* ------------------------------------------ MEMBERS -------------------------------------------*/
	UPROPERTY(EditAnywhere)
	TSubclassOf<ACCCard> CardPrefab;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UDataTable> CardDataTable;
	
	UPROPERTY(EditAnywhere)
	TArray<FName> DeckCards;

	UPROPERTY(EditAnywhere)
	FVector DeckPosition;
	
	/* -----------------------------------------FUNCTIONS -------------------------------------------*/
	ACCCard* CreateCard();

	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
protected:
	virtual void BeginPlay() override;
};
