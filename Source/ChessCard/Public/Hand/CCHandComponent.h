#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "CCHandComponent.generated.h"


class ACCCard;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class CHESSCARD_API UCCHandComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCCHandComponent();

	/* ------------------------------------------ MEMBERS -------------------------------------------*/
public:
	UPROPERTY()
	TArray<TObjectPtr<ACCCard>> Cards;

	/* ------------------------------------------ FUNCTIONS -------------------------------------------*/
public:
	UFUNCTION()
	int GetCardNum(){return Cards.Num();}

	/* ------------------------------------------ OVERRIDES -------------------------------------------*/
protected:
	virtual void BeginPlay() override;
	
	/* ------------------------------------------ GETTERS/SETTERS -------------------------------------------*/
};
