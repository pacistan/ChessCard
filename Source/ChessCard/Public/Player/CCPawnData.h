#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "CCPawnData.generated.h"

UCLASS()
class CHESSCARD_API UCCPawnData : public UDataAsset
{
	GENERATED_BODY()
public:
	// Class to instantiate for this pawn (should usually derive from ALyraPawn or ALyraCharacter).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "CC|Pawn")
	TSubclassOf<APawn> PawnClass;
	
	// Maybe Camera Mode ?

	// Maybe Input Config ?

public:
	UCCPawnData(const FObjectInitializer& ObjectInitializer);
};
