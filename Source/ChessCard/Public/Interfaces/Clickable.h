#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Clickable.generated.h"

class ACCPlayerPawn;

UINTERFACE()
class UClickable : public UInterface
{
	GENERATED_BODY()
};

class CHESSCARD_API IClickable
{
	GENERATED_BODY()

public:
	virtual void Click(ACCPlayerPawn* Player) = 0;
};
