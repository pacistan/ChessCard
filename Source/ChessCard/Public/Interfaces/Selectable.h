#pragma once

#include "CoreMinimal.h"
#include "Hoverable.h"
#include "UObject/Interface.h"
#include "Selectable.generated.h"

class ACCPlayerPawn;

UINTERFACE()
class USelectable : public UInterface
{
	GENERATED_BODY()
};

class CHESSCARD_API ISelectable
{
	GENERATED_BODY()

public:
	virtual void Select(ACCPlayerPawn* Player) = 0;
	virtual void UnSelect(ACCPlayerPawn* Player) = 0;

};
