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
private:
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
public:
	UFUNCTION(BlueprintGetter)
	TArray<ACCCard*> GetCards(){return Cards;}
};
