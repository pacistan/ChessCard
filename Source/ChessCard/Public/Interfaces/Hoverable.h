#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "Hoverable.generated.h"

class ACCPlayerPawn;

UINTERFACE()
class UHoverable : public UInterface
{
	GENERATED_BODY()
};


class CHESSCARD_API IHoverable
{
	GENERATED_BODY()

public:
	virtual void StartHover(ACCPlayerPawn* Player) = 0;
	virtual void StopHover(ACCPlayerPawn* Player) = 0;
	virtual bool GetCardData(FDataTableRowHandle& OutRowHandle) { return false; }
};
